#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <climits>

#define TASK_ONE 1
#define TASK_TWO 2
#define TASK_THREE 3
#define ARRIVABLE true
#define UNARRIVABLE false

// 2 * 10^8
#define MAX_W 2 * 100000000

// 2 * 10^4
#define TASK_ONE_MAX_K 2 * 10000

using namespace std;

class Task {
public:
    int n, m, k, l, s;
    vector<int> f;
    vector<vector<pair<int, char>>> adj;
    string result;

    void read_input(ifstream& fin) {
        fin >> n >> l >> m >> k >> s;

        adj.resize(n + 1);        
        f.resize(m);

        for (int i = 0; i < m; ++i) {
            fin >> f[i];
        }

        int x;
        for (int i = 1; i <= n; i++) {
            for (int j = 0; j < l; ++j) {
                fin >> x;
                adj[i].push_back({x, 'a' + ((char) j)});
            }
        }
    }

    void print_output(ofstream& fout) {
        if (result == "-1") {
            fout << "-1";
        } else {
            fout << result.size() << '\n';
            fout << result;
        }
    
    }

    virtual void solve() {
    }
};

class TaskOne : public Task {
public:
    long long steps_max_limit = 0;
    vector<vector<pair<bool,int>>>dp;
    vector<vector<pair<int, char>>> t_adj; // transposed adjacency matrix

    void initialize_dp_matrix() {
        steps_max_limit = TASK_ONE_MAX_K + 2 * n;

        dp.resize(steps_max_limit + 1);
        for (int i = 0; i <= steps_max_limit; ++i) {
            dp[i].resize(n + 1);
            for (int j = 1; j <= n; ++j) {
                dp[i][j] = {UNARRIVABLE, 0};
            }
        }
    
        for (int i = 0; i < m; ++i) {
            dp[0][f[i]] = {ARRIVABLE, 0};
        }
    }

    void initialize_transposed_adjacency_matrix() {
        t_adj.resize(n + 1);

        for (int i = 1; i <= n; ++i) {
            for (auto& edge : adj[i]) {
                t_adj[edge.first].push_back({i, edge.second});
            }
        }
    }

    void solve() override {
        initialize_dp_matrix();
        initialize_transposed_adjacency_matrix();

        // step for
        for (int i = 0; i < steps_max_limit; i++) {
            if (dp[i][s].first == ARRIVABLE && dp[i][s].second > 0 && i >= k) {
                r = i;
                break;
            }

            if (i == steps_max_limit) {
                break;
            }
            int next_step = i + 1;

            // node for
            for (int j = 1; j <= n; j++) {
                if (dp[i][j].first == UNARRIVABLE) {
                    continue;
                }

                // adj for
                for (auto& edge : t_adj[j]) {
                    if (dp[next_step][edge.first].first == UNARRIVABLE) {
                        dp[next_step][edge.first] = {ARRIVABLE, j};
                    }
                }
            }
        }

        // store the result in the result string variable
        if (r == -1) {
            result = "-1";
        } else {
            int c = s, p;

            // traverse the path from r to 0 and reverse the string
            while (r) {
                // stiu nodul curent, stiu nodul parinte, ma folosesc de adj[curr][parent].second
                p = dp[r--][c].second;

                int adj_p = 0;
                for (auto& edge : adj[c]) {
                    if (edge.first == p) {
                        adj_p = edge.second;
                        break;
                    }
                }

                result += adj_p;
                c = p;
            }
        }
    }
};

class TaskTwo : public Task {
public:
    void solve() override {
    }
};

class TaskThree : public Task {
public:
    void solve() override {
    }
};

Task* getTask(int taskType) {
    if (taskType == TASK_ONE) {
        return new TaskOne();
    } else if (taskType == TASK_TWO) {
        return new TaskTwo();
    } else if (taskType == TASK_THREE) {
        return new TaskThree();
    } else {
        return new Task();
    }
}

int main() {
    ifstream inputFile("input.txt");
    ofstream outputFile("output.txt");

    int taskType;
    inputFile >> taskType;

    Task* task = getTask(taskType);
    task->read_input(inputFile);
    task->solve();
    task->print_output(outputFile);

    inputFile.close();
    outputFile.close();

    return 0;
}
