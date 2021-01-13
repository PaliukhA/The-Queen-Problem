#include<bits/stdc++.h>
#pragma GCC optimize("O3")
using namespace std;

typedef long long ll;
typedef long double ld;

const int max_size = 4e4;
const int shift = 1e4;
int cnt[6][max_size];

const int first_arr[6] = {1, 1, 2, 1, 2, -1};
const int second_arr[6] = {1, -1, 1, 2, -1, 2};

int SwapScore(int left_x, int left_y, int right_x, int right_y) {
    int score = 0;
    for (int i = 0; i < 6; ++i) {
        if (--cnt[i][first_arr[i] * left_x + second_arr[i] * left_y + shift] > 0) {
            score++;
        }
        if (--cnt[i][first_arr[i] * right_x + second_arr[i] * right_y + shift] > 0) {
            score++;
        }
    }
    for (int i = 0; i < 6; ++i) {
        if (++cnt[i][first_arr[i] * left_x + second_arr[i] * right_y + shift] > 1) {
            score--;
        }
        if (++cnt[i][first_arr[i] * right_x + second_arr[i] * left_y + shift] > 1) {
            score--;
        }
    }
    return score;
}

void Swap(int left_x, int left_y, int right_x, int right_y) {
    for (int i = 0; i < 6; ++i) {
        --cnt[i][first_arr[i] * left_x + second_arr[i] * left_y + shift];
        --cnt[i][first_arr[i] * right_x + second_arr[i] * right_y + shift];
        ++cnt[i][first_arr[i] * left_x + second_arr[i] * right_y + shift];
        ++cnt[i][first_arr[i] * right_x + second_arr[i] * left_y + shift];
    }
}

void GoodFinish(const vector<int> &result) {
    cout << "YES\n";
    for (int cur: result) {
        cout << ++cur << '\n';
    }
}

double temp;
int cnt_iter;
double coef_cahge_temp = 0.9999998;
bool change_type_temp = false;

void GetParamAnnealing(int n) {
    if (n < 500) {
        cnt_iter = 64e6;
    } else if (n < 1000) {
        cnt_iter = 7e7;
        change_type_temp = true;
    } else if (n < 5000) {
        cnt_iter = 59e6;
    } else {
        cnt_iter = 47e7;
    }
    if (n < 70) {
        temp = 0.24;
    } else if (n < 100) {
        temp = 0.2;
    } else if (n < 150) {
        temp = 0.1756;
    } else if (n < 200) {
        temp = 0.176;
    } else {
        temp = 0.091;
    }
    double d = 0.1;
    while (pow(1 - d, cnt_iter) < 1e-3) {
        d *= 0.1;
    }
    d *= 10;
    coef_cahge_temp = 1.0 - d;
}

int GetPenalty(int x, int y) {
    int cur_penalty = 0;
    for (int j = 0; j < 6; ++j) {
        if (cnt[j][first_arr[j] * x + second_arr[j] * y + shift] > 1) {
            ++cur_penalty;
        }
    }
    return cur_penalty;
}

bool Annealing(const vector<int> &not_using_x, vector<int> &not_using_y, const vector<char> &not_const,
               vector<int> &perm, int n) {
    mt19937 gen(235);
    int penalty = 0;
    for (int i = 0; i < 6; ++i) {
        for (int j = 0; j < max_size; ++j) {
            cnt[i][j] = 0;
        }
    }
    for (int i = 0; i < n; ++i) {
        if (!not_const[i]) {
            for (int j = 0; j < 6; ++j) {
                int collision = ++cnt[j][first_arr[j] * i + second_arr[j] * perm[i] + shift];
            }
        }
    }
    shuffle(not_using_y.begin(), not_using_y.end(), gen);
    auto it = not_using_y.begin();
    for (int i = 0; i < n; ++i) {
        if (not_const[i]) {
            perm[i] = *it;
            ++it;
            for (int j = 0; j < 6; ++j) {
                int collision = ++cnt[j][first_arr[j] * i + second_arr[j] * perm[i] + shift];
                if (collision > 1) {
                    ++penalty;
                }
            }
        }
    }
    size_t not_using_x_size = not_using_x.size();
    uniform_real_distribution<double> distr(0, 1);
    double cur_temp = 1;
    for (int iter = 0; iter < cnt_iter; ++iter) {
        if (penalty == 0) {
            GoodFinish(perm);
            return true;
        }
        int left = not_using_x[iter % not_using_x_size];
        int right = not_using_x[gen() % not_using_x_size];
        while (left == right) {
            right = not_using_x[gen() % not_using_x_size];
        }
        int swap_score = SwapScore(left, perm[left], right, perm[right]);
        if (swap_score >= 0) {
            swap(perm[left], perm[right]);
            penalty -= swap_score;
        } else {
            double prob;
            if (change_type_temp) {
                prob = exp(swap_score / cur_temp);
            } else {
                prob = exp(swap_score / temp);
            }
            if (distr(gen) < prob) {
                swap(perm[left], perm[right]);
                penalty -= swap_score;
            } else {
                Swap(left, perm[right], right, perm[left]);
            }
        }
        cur_temp *= coef_cahge_temp;
    }
    return false;
}

struct Node {
    int penalty;
    int random;
    int idx;
};

bool operator<(const Node &lhs, const Node &rhs) {
    return lhs.penalty < rhs.penalty ||
        (lhs.penalty == rhs.penalty && lhs.random < rhs.random);
}


bool LocalSearch(const vector<int> &not_using_x, vector<int> &not_using_y, const vector<char> &not_const,
                 vector<int> &perm, int n) {
    mt19937 gen(235);
    int penalty = 0;
    for (int i = 0; i < 6; ++i) {
        for (int j = 0; j < max_size; ++j) {
            cnt[i][j] = 0;
        }
    }
    for (int i = 0; i < n; ++i) {
        if (!not_const[i]) {
            for (int j = 0; j < 6; ++j) {
                ++cnt[j][first_arr[j] * i + second_arr[j] * perm[i] + shift];
            }
        }
    }
    shuffle(not_using_y.begin(), not_using_y.end(), gen);
    auto it = not_using_y.begin();
    for (int i = 0; i < n; ++i) {
        if (not_const[i]) {
            perm[i] = *it;
            ++it;
            for (int j = 0; j < 6; ++j) {
                int collision = ++cnt[j][first_arr[j] * i + second_arr[j] * perm[i] + shift];
                if (collision > 1) {
                    ++penalty;
                }
            }
        }
    }
    size_t not_using_x_size = not_using_x.size();
    uniform_real_distribution<double> distr(0, 1);
    uniform_int_distribution<int> int_distr(0, 1073741824);
    double cur_temp = 1;
    priority_queue<Node> set_node;
    for (int i = 0; i < n; ++i) {
        if (not_const[i]) {
            set_node.push(Node{GetPenalty(i, perm[i]), int_distr(gen), i});
        }
    }
    for (int iter = 0; iter < cnt_iter; ++iter) {
        if (penalty == 0) {
            GoodFinish(perm);
            return true;
        }
        int left = set_node.top().idx;
        set_node.pop();
        int idx = not_using_x[iter % not_using_x_size];
        int right;
        right = set_node.top().idx;
        set_node.pop();
        if (idx != left) {
            swap(right, idx);
        }
        int swap_score = SwapScore(left, perm[left], right, perm[right]);
        if (swap_score >= 0) {
            swap(perm[left], perm[right]);
            penalty -= swap_score;
        } else {
            double prob;
            if (change_type_temp) {
                prob = exp(swap_score / cur_temp);
            } else {
                prob = exp(swap_score / temp);
            }
            if (distr(gen) < prob) {
                swap(perm[left], perm[right]);
                penalty -= swap_score;
            } else {
                Swap(left, perm[right], right, perm[left]);
            }
        }
        set_node.push(Node{GetPenalty(left, perm[left]), int_distr(gen), left});
        set_node.push(Node{GetPenalty(right, perm[right]), int_distr(gen), right});
        cur_temp *= coef_cahge_temp;
    }
    return false;
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    cout << setprecision(20);
    cout << fixed;
    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);
    auto start = std::chrono::steady_clock::
    now().time_since_epoch();
    int n, k;
    cin >> n >> k;
    if (n == 1) {
        cout << "YES\n1";
        return 0;
    }
    if (n <= 4) {
        cout << "NO";
        return 0;
    }
    vector<char> not_const(n, 1);
    vector<int> perm(n, -1);
    set<int> collision_row;
    set<int> collision_column;
    set<int> not_using_y_set;
    vector<int> not_using_x;
    vector<int> not_using_y;
    for (int i = 0; i < n; ++i) {
        not_using_y_set.insert(i);
    }
    for (int i = 0; i < k; ++i) {
        int x, y;
        cin >> x >> y;
        x--;
        y--;
        for (int j = 0; j < 6; ++j) {
            int collision = ++cnt[j][first_arr[j] * x + second_arr[j] * y + shift];
            if (collision > 1) {
                cout << "NO";
                return 0;
            }
        }
        perm[x] = y;
        collision_row.insert(x);
        collision_column.insert(y);
        not_const[x] = 0;
        not_using_y_set.erase(y);
    }
    if (collision_row.size() < k) {
        cout << "NO";
        return 0;
    }
    if (collision_column.size() < k) {
        cout << "NO\n";
        return 0;
    }
    for (int y : not_using_y_set) {
        not_using_y.push_back(y);
    }
    for (int i = 0; i < n; ++i) {
        if (not_const[i]) {
            not_using_x.push_back(i);
        }
    }
    GetParamAnnealing(n);
    if( n < 2000) {
        if (!Annealing(not_using_x, not_using_y, not_const, perm, n)) {
            cout << "NO";
        }
    } else {
        if (!LocalSearch(not_using_x, not_using_y, not_const, perm, n)) {
            cout << "NO";
        }
    }
    return 0;
}
