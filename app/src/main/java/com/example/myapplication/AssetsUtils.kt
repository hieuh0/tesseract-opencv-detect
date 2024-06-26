package com.example.myapplication

import android.content.Context
import android.content.res.AssetManager
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import androidx.annotation.Nullable
import java.io.File
import java.io.FileOutputStream
import java.io.IOException


object AssetsUtils {
    fun getLocalDir(context: Context): File {
        return context.filesDir
    }

    /**
     * Returns locally accessible directory path which contains the "tessdata" subdirectory
     * with *.traineddata files.
     */
    fun getTessDataPath(context: Context): String {
        return getLocalDir(context).absolutePath
    }

    fun getImageFile(context: Context): File {
        return File(getLocalDir(context), "screenshot_20240614_090845.png")
    }

    fun getImageBitmap(context: Context): Bitmap {
        return BitmapFactory.decodeFile(getImageFile(context).absolutePath)
    }

    fun extractAssets(context: Context) {
        val am = context.assets

        val localDir = getLocalDir(context)
        if (!localDir.exists() && !localDir.mkdir()) {
            throw RuntimeException("Can't create directory $localDir")
        }

        val tessDir = File(getTessDataPath(context), "tessdata")
        if (!tessDir.exists() && !tessDir.mkdir()) {
            throw RuntimeException("Can't create directory $tessDir")
        }

        // Extract all assets to our local directory.
        // All *.traineddata into "tessdata" subdirectory, other files into root.
        try {
            for (assetName in am.list("")!!) {
                val targetFile = if (assetName.endsWith(".traineddata")) {
                    File(tessDir, assetName)
                } else {
                    File(localDir, assetName)
                }
                if (!targetFile.exists()) {
                    copyFile(am, assetName, targetFile)
                }
            }
        } catch (e: IOException) {
            e.printStackTrace()
        }
    }

    private fun copyFile(
        am: AssetManager, assetName: String,
        outFile: File
    ) {
        try {
            am.open(assetName).use { `in` ->
                FileOutputStream(outFile).use { out ->
                    val buffer = ByteArray(1024)
                    var read: Int
                    while ((`in`.read(buffer).also { read = it }) != -1) {
                        out.write(buffer, 0, read)
                    }
                }
            }
        } catch (e: IOException) {
            e.printStackTrace()
        }
    }
}