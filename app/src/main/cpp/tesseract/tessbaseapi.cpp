/*
 * Copyright (C) 2019 Adaptech s.r.o., Robert Pösel
 * Copyright 2011, Google Inc.
 * Copyright 2011, Robert Theis
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <malloc.h>
#include "android/bitmap.h"
#include "common.h"
#include <tesseract/baseapi.h>
#include <tesseract/ocrclass.h>
#include "allheaders.h"
#include <tesseract/renderer.h>
#include <opencv2/opencv.hpp>

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_myapplication_MainActivity_findTextCoordinates(JNIEnv *env, jobject thiz, jobject bitmap, jstring input_text, jstring datapath ) {
    const char *input_text_char = env->GetStringUTFChars(input_text, nullptr);

    // Initialize OpenCV structures
    AndroidBitmapInfo info;
    void *pixels;
    if (AndroidBitmap_getInfo(env, bitmap, &info) < 0) {
        return env->NewStringUTF("Failed to get bitmap info.");
    }
    if (AndroidBitmap_lockPixels(env, bitmap, &pixels) < 0) {
        return env->NewStringUTF("Failed to lock bitmap pixels.");
    }

    // Convert Android Bitmap to OpenCV Mat
    cv::Mat img(info.height, info.width, CV_8UC4, pixels);
    cv::Mat gray;
    cv::cvtColor(img, gray, cv::COLOR_RGBA2GRAY);
    AndroidBitmap_unlockPixels(env, bitmap);

    // Image preprocessing
    cv::Mat thresh;
    cv::threshold(gray, thresh, 0, 255, cv::THRESH_OTSU | cv::THRESH_BINARY_INV);
    cv::Mat rect_kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(18, 18));
    cv::Mat dilation;
    cv::dilate(thresh, dilation, rect_kernel, cv::Point(-1, -1), 1);

    // Find contours
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(dilation, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

    // Initialize Tesseract
    tesseract::TessBaseAPI tess;
    const char *datapath_char = env->GetStringUTFChars(datapath, nullptr);
    if (tess.Init(datapath_char, "eng", tesseract::OEM_DEFAULT) != 0) {
        LOGE("Failed to initialize Tesseract.");
        return env->NewStringUTF("Failed to initialize Tesseract.");
    }
    tess.SetPageSegMode(tesseract::PSM_SINGLE_BLOCK);

    bool found = false;
    std::string result_text;

    for (const auto &contour : contours) {
        cv::Rect bounding_rect = cv::boundingRect(contour);
        cv::Mat cropped = gray(bounding_rect);
        std::string image_save_path = std::string(datapath_char) + "/cropped_image.png";

        // Save the cropped image for inspection
        if (!cv::imwrite(image_save_path, cropped)) {
            LOGE("Failed to save cropped image to: %s", image_save_path.c_str());
        }

        // Save the cropped image for inspection
        cv::imwrite(image_save_path, cropped);

        tess.SetImage(cropped.data, cropped.cols, cropped.rows, 1, cropped.step);
        char *text = tess.GetUTF8Text();
        if (text == nullptr) {
            continue;
        }
        std::string recognized_text(text);
        delete[] text; // Properly delete the text after use

        if (recognized_text.find(input_text_char) != std::string::npos) {
            cv::rectangle(img, bounding_rect, cv::Scalar(0, 255, 0), 2);  // Draw rectangle around found text
            result_text = "Location of input text: x = " + std::to_string(bounding_rect.x) + " y = " + std::to_string(bounding_rect.y);
            found = true;
            break;
        }
    }

    tess.End();
    env->ReleaseStringUTFChars(input_text, input_text_char);

    if (!found) {
        result_text = "Input text not found in the image.";
    }

    return env->NewStringUTF(result_text.c_str());
}