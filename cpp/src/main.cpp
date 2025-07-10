#include <iostream>
#include <vector>
#include <string>

// パラメータを保持する構造体
struct Parameters {
    int spd = 0;
    int sta = 0;
    int pow = 0;
    int gut = 0;
    int wis = 0;
};

// "good"評価の閾値
const int SPD_THRESHOLD = 1800;
const int STA_THRESHOLD = 1500;
const int POW_THRESHOLD = 1600;
const int GUT_THRESHOLD = 1200;
const int WIS_THRESHOLD = 1200;

// パラメータが"good"評価基準を満たすかチェックする関数
bool is_good(const Parameters& params) {
    return params.spd >= SPD_THRESHOLD &&
           params.sta >= STA_THRESHOLD &&
           params.pow >= POW_THRESHOLD &&
           params.gut >= GUT_THRESHOLD &&
           params.wis >= WIS_THRESHOLD;
}

// 1ターンのトレーニング後に"good"評価になる最適な練習を検索する関数
std::vector<std::string> find_best_trainings(const Parameters& current_params) {
    std::vector<std::string> best_trainings;

    // 各トレーニングをシミュレーション
    // 1. Tspd
    Parameters after_tspd = current_params;
    after_tspd.spd += 50;
    if (is_good(after_tspd)) {
        best_trainings.push_back("Tspd");
    }

    // 2. Tsta
    Parameters after_tsta = current_params;
    after_tsta.sta += 50;
    if (is_good(after_tsta)) {
        best_trainings.push_back("Tsta");
    }

    // 3. Tpow
    Parameters after_tpow = current_params;
    after_tpow.pow += 50;
    if (is_good(after_tpow)) {
        best_trainings.push_back("Tpow");
    }

    // 4. Tgut
    Parameters after_tgut = current_params;
    after_tgut.gut += 50;
    if (is_good(after_tgut)) {
        best_trainings.push_back("Tgut");
    }

    // 5. Twis
    Parameters after_twis = current_params;
    after_twis.wis += 50;
    if (is_good(after_twis)) {
        best_trainings.push_back("Twis");
    }

    return best_trainings;
}

int main() {
    // 現在のパラメータ（例）
    // パラメータは50刻みで与えられる想定
    Parameters current_params = {1750, 1500, 1600, 1200, 1200};

    // 最適な練習を検索
    std::vector<std::string> best = find_best_trainings(current_params);

    // 結果を出力
    std::cout << "Current Parameters:" << std::endl;
    std::cout << "  SPD: " << current_params.spd << std::endl;
    std::cout << "  STA: " << current_params.sta << std::endl;
    std::cout << "  POW: " << current_params.pow << std::endl;
    std::cout << "  GUT: " << current_params.gut << std::endl;
    std::cout << "  WIS: " << current_params.wis << std::endl;
    std::cout << std::endl;

    if (best.empty()) {
        std::cout << "No training will result in a 'good' evaluation in 1 turn." << std::endl;
    } else {
        std::cout << "Best training(s) to get a 'good' evaluation in 1 turn:" << std::endl;
        for (const auto& training : best) {
            std::cout << "  - " << training << std::endl;
        }
    }

    return 0;
}