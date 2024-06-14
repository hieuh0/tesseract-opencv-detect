package com.example.myapplication

import android.graphics.Bitmap
import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import androidx.core.content.ContentProviderCompat.requireContext
import androidx.core.content.ContextCompat
import androidx.core.graphics.drawable.toBitmap
import com.example.myapplication.databinding.ActivityMainBinding


class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)
        AssetsUtils.extractAssets(this);
        val bitmap = ContextCompat.getDrawable(this, R.drawable.screenshot_20240613_214905)?.toBitmap() ?: return
//        // Example of a call to a native method
        val dataPath: String = AssetsUtils.getTessDataPath(this)
//        /data/data/com.example.myapplication/files
        binding.sampleText.text = findTextCoordinates(bitmap, "Gmail", "$dataPath/tessdata/")
    }

    /**
     * A native method that is implemented by the 'myapplication' native library,
     * which is packaged with this application.
     */
//    private external fun adaptiveThresholdFromJNI(): String
    private external fun findTextCoordinates(bitmap: Bitmap, inputText: String, datapath: String): String


    companion object {
        // Used to load the 'myapplication' library on application startup.
        init {
            System.loadLibrary("jpeg");
            System.loadLibrary("pngx");
            System.loadLibrary("leptonica");
            System.loadLibrary("tesseract");

        }
    }
}