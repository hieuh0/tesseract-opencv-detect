//#include <jni.h>
//#include <opencv2/opencv.hpp>
//#include <tesseract/baseapi.h>
//#include <android/bitmap.h>
////
//// Created by Admin-PC on 6/13/2024.
////
//
//extern "C"
//JNIEXPORT jstring JNICALL
//Java_com_example_myapplication_MainActivity_adaptiveThresholdFromJNI(JNIEnv *env, jobject thiz) {
//    return env->NewStringUTF("\"Input text not found in the image.\"");
//}
//extern "C"
//JNIEXPORT jstring JNICALL
//Java_com_example_myapplication_MainActivity_findTextCoordinates(JNIEnv *env, jobject thiz,
//                                                                jobject bitmap,
//                                                                jstring input_text) {
//    const char *input_text_char = env->GetStringUTFChars(input_text, nullptr);
//
//    // Convert Java Bitmap to OpenCV Mat
//    AndroidBitmapInfo info;
//    void *pixels;
//    AndroidBitmap_getInfo(env, bitmap, &info);
//    AndroidBitmap_lockPixels(env, bitmap, &pixels);
//    cv::Mat img(info.height, info.width, CV_8UC4, pixels);
//    cv::Mat gray;
//    cv::cvtColor(img, gray, cv::COLOR_RGBA2GRAY);
//    AndroidBitmap_unlockPixels(env, bitmap);
//
//    // Preprocessing
//    cv::Mat thresh;
//    cv::threshold(gray, thresh, 0, 255, cv::THRESH_OTSU | cv::THRESH_BINARY_INV);
//
//    cv::Mat rect_kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(18, 18));
//    cv::Mat dilation;
//    cv::dilate(thresh, dilation, rect_kernel, cv::Point(-1, -1), 1);
//
//    std::vector<std::vector<cv::Point>> contours;
//    cv::findContours(dilation, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
//
//    tesseract::TessBaseAPI tess;
//    tess.Init(NULL, "eng", tesseract::OEM_DEFAULT);
//    tess.SetPageSegMode(tesseract::PSM_SINGLE_BLOCK);
//
//    bool found = false;
//    std::string result_text;
//
//    for (const auto &contour : contours) {
//        cv::Rect bounding_rect = cv::boundingRect(contour);
//        cv::Mat cropped = gray(bounding_rect);
//
//        tess.SetImage(cropped.data, cropped.cols, cropped.rows, 1, cropped.step);
//        char *text = tess.GetUTF8Text();
//        std::string recognized_text(text);
//        tess.DeleteText(text);
//
//        if (recognized_text.find(input_text_char) != std::string::npos) {
//            cv::rectangle(img, bounding_rect, cv::Scalar(0, 255, 0), 2);  // Draw rectangle around found text
//            result_text = "Location of input text: x = " + std::to_string(bounding_rect.x) + " y = " + std::to_string(bounding_rect.y);
//            found = true;
//            break;
//        }
//    }
//
//    tess.End();
//    env->ReleaseStringUTFChars(input_text, input_text_char);
//
//    if (!found) {
//        result_text = "Input text not found in the image.";
//    }
//
//    return env->NewStringUTF(result_text.c_str());
//}