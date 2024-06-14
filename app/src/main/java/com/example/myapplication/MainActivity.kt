package com.example.myapplication

import android.graphics.Bitmap
import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import androidx.lifecycle.lifecycleScope
import com.example.myapplication.databinding.ActivityMainBinding
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.GlobalScope
import kotlinx.coroutines.launch
import java.util.concurrent.ExecutorService
import java.util.concurrent.Executors
import kotlin.coroutines.resume
import kotlin.coroutines.suspendCoroutine


class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding
    private val executor: ExecutorService = Executors.newSingleThreadExecutor()
    fun findTextCoordinatesInBackground(bitmap: Bitmap?, inputText: String?, dataPath: String?) {
        executor.submit {
            val result = findTextCoordinates(bitmap!!, inputText!!, dataPath!!)
            // Xử lý kết quả ở đây, ví dụ cập nhật UI
            runOnUiThread {
                // Cập nhật UI với kết quả
                binding.sampleText.text = result.status.toString()
            }
        }
    }
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)
        AssetsUtils.extractAssets(this);

//        // Example of a call to a native method
        val dataPath: String = AssetsUtils.getTessDataPath(this)
//        /data/data/com.example.myapplication/files
        val bitmap = AssetsUtils.getImageBitmap(this)
//        lifecycleScope.launch {
//            binding.sampleText.text =
//        }
        findTextCoordinatesInBackground(bitmap, "24H | OFFICIAL", "$dataPath/tessdata/")
    }

    /**
     * A native method that is implemented by the 'myapplication' native library,
     * which is packaged with this application.
     */
//    private external fun adaptiveThresholdFromJNI(): String
    private external fun findTextCoordinates(bitmap: Bitmap, inputText: String, datapath: String): TextResult


    suspend fun findTextCoordinates1(bitmap: Bitmap, inputText: String, datapath: String): TextResult =
        suspendCoroutine { continuation ->
            GlobalScope.launch(Dispatchers.IO) {
                val result = findTextCoordinates(bitmap, inputText, datapath)
                continuation.resume(result)
            }
        }

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