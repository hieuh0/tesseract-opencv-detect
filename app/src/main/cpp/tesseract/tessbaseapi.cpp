#include <jni.h>
#include <android/bitmap.h>
#include <android/log.h>
#include <opencv2/opencv.hpp>
#include <tesseract/baseapi.h>
#include <string>
#include <chrono>
#include <thread>
#include <vector>
#include <mutex>

#define LOG_TAG "NativeLib"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

extern "C" {

JNIEXPORT jobject JNICALL
Java_com_example_myapplication_MainActivity_findTextCoordinates(JNIEnv *env, jobject thiz,
                                                                jobject bitmap, jstring input_text,
                                                                jstring datapath) {
    auto start_time = std::chrono::steady_clock::now();

    AndroidBitmapInfo info;
    void *pixels = nullptr;
    int ret;

    const char *input_text_char = env->GetStringUTFChars(input_text, nullptr);
    const char *datapath_char = env->GetStringUTFChars(datapath, nullptr);

    if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {
        LOGE("AndroidBitmap_getInfo() failed: %d", ret);
        return nullptr;
    }

    if ((ret = AndroidBitmap_lockPixels(env, bitmap, &pixels)) < 0) {
        LOGE("AndroidBitmap_lockPixels() failed: %d", ret);
        return nullptr;
    }

    cv::Mat img(info.height, info.width, CV_8UC4, pixels);
    cv::Mat gray;
    cv::cvtColor(img, gray, cv::COLOR_RGBA2GRAY);

    cv::Mat thresh;
    cv::threshold(gray, thresh, 0, 255, cv::THRESH_OTSU | cv::THRESH_BINARY_INV);
    cv::Mat rect_kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(18, 18));
    cv::Mat dilation;
    cv::dilate(thresh, dilation, rect_kernel, cv::Point(-1, -1), 1);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(dilation, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

    bool found = false;
    int result_x = -1;
    int result_y = -1;

    int num_threads = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;
    std::mutex mutex;

    auto process_contour = [&](int start, int end) {
        tesseract::TessBaseAPI tess;
        if (tess.Init(datapath_char, "eng", tesseract::OEM_LSTM_ONLY) != 0) {
            LOGE("Failed to initialize Tesseract.");
            return;
        }
        tess.SetPageSegMode(tesseract::PSM_AUTO);

        for (int i = start; i < end; ++i) {
            if (found) continue;

            cv::Rect bounding_rect = cv::boundingRect(contours[i]);
            cv::Mat cropped = gray(bounding_rect);

            tess.SetImage(cropped.data, cropped.cols, cropped.rows, 1, cropped.step);
            char *text = tess.GetUTF8Text();

            if (text != nullptr && text[0] != '\0') {
                std::string recognized_text(text);
                delete[] text; // Giải phóng bộ nhớ sau khi sử dụng
                LOGI("Recognized text: %s", recognized_text.c_str());
                if (recognized_text.find(input_text_char) != std::string::npos) {
                    std::lock_guard<std::mutex> lock(mutex);
                    if (!found) {
                        result_x = bounding_rect.x;
                        result_y = bounding_rect.y;
                        found = true;
                    }
                }
            }
        }
        tess.End();
    };

    int contours_per_thread = contours.size() / num_threads;
    for (int i = 0; i < num_threads; ++i) {
        int start = i * contours_per_thread;
        int end = (i == num_threads - 1) ? contours.size() : (i + 1) * contours_per_thread;
        threads.emplace_back(process_contour, start, end);
    }
    for (auto &thread : threads) {
        thread.join();
    }

    AndroidBitmap_unlockPixels(env, bitmap);
    env->ReleaseStringUTFChars(input_text, input_text_char);
    env->ReleaseStringUTFChars(datapath, datapath_char);

    auto end_time = std::chrono::steady_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    LOGI("Processing time: %lld milliseconds", elapsed_time);

    jclass result_class = env->FindClass("com/example/myapplication/TextResult");
    if (result_class == nullptr) {
        LOGE("Failed to find TextResult class");
        return nullptr;
    }
    jmethodID constructor = env->GetMethodID(result_class, "<init>", "(ZII)V");
    if (constructor == nullptr) {
        LOGE("Failed to find TextResult constructor");
        return nullptr;
    }

    jobject result_object = env->NewObject(result_class, constructor, found, result_x, result_y);
    if (result_object == nullptr) {
        LOGE("Failed to create TextResult object");
        return nullptr;
    }

    return result_object;
}

}
