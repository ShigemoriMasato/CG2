#include "FPSObserver.h"
#pragma comment(lib,"winmm.lib")

bool FPSObserver::isFix_ = true;
float FPSObserver::deltatime_ = 1.0f / 60.0f;

FPSObserver::FPSObserver(bool isfix, double targetfps) : targetFPS_(targetfps), frameTime_(1.0 / targetfps) {
    // 周波数取得
    QueryPerformanceFrequency(&timeFreq_);
    // 計測開始時間の初期化
    QueryPerformanceCounter(&timeStart_);

    // 高精度タイマーの設定
    timeBeginPeriod(1);

    isFix_ = isfix;
}

FPSObserver::~FPSObserver() {
    // 高精度タイマーの解除
    timeEndPeriod(1);
}

void FPSObserver::SetTargetFPS(double targetfps) {
    targetFPS_ = targetfps;
	frameTime_ = 1.0 / targetfps;
}

void FPSObserver::TimeAdjustment() {
    // フレーム終了時間を取得
    QueryPerformanceCounter(&timeEnd_);

    // 経過時間を計算（秒単位）
    double frameTime = static_cast<double>(timeEnd_.QuadPart - timeStart_.QuadPart) /
        static_cast<double>(timeFreq_.QuadPart);

    if (isFix_) {

        // 目標フレーム時間まで待機
        if (frameTime < frameTime_) {
            double remainingTime = frameTime_ - frameTime;
            PreciseSleep(remainingTime);

            // 待機後の実際の時間を再測定
            QueryPerformanceCounter(&timeEnd_);
            frameTime = static_cast<double>(timeEnd_.QuadPart - timeStart_.QuadPart) /
                static_cast<double>(timeFreq_.QuadPart);
        }

        deltatime_ = static_cast<float>(frameTime);
    }

    // 次のフレームの開始時間を設定
    timeStart_ = timeEnd_;
}

void FPSObserver::PreciseSleep(double seconds) {
    if (seconds <= 0.0) return;

    LARGE_INTEGER sleepStart, sleepCurrent;
    QueryPerformanceCounter(&sleepStart);

    double targetTicks = seconds * static_cast<double>(timeFreq_.QuadPart);

    // 粗い待機
    if (seconds > 0.002) {
        DWORD sleepMs = static_cast<DWORD>((seconds - 0.001) * 1000.0);
        Sleep(sleepMs);
    }

    // 精密待機
    do {
        QueryPerformanceCounter(&sleepCurrent);
    } while ((sleepCurrent.QuadPart - sleepStart.QuadPart) < targetTicks);
}
