#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <climits>
#include <algorithm>

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
        steps_max_limit = TASK_ONE_MAX_K + n;

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

        int r = -1;

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
                // i know the current node, i know the parent node
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
    vector<pair<int, vector<vector<int>>>> ths; // throughs (intermediates)
    vector<pair<int, vector<vector<bool>>>> adjs;
    vector<pair<int, pair<int, int>>> th_components; // how are the ths pows composed
    string rres, lres;
    int min_bfs, bfs_src_node;

    void multiply(vector<vector<bool>>& a, vector<vector<bool>>& b, int _s) {
        // Creating an auxiliary matrix to store elements
        vector<vector<bool>> m(n + 1, vector<bool>(n + 1));

        // Get the address and intermediate matrix from th
        pair<int, vector<vector<int>>> th = {_s, vector<vector<int>>(n + 1, vector<int>(n + 1, 0))};

        // Calculating if there is a path of length _s from i to j
        for (int i = 1; i <= n; i++) {
            for (int j = 1; j <= n; j++) {
                m[i][j] = false;
                for (int k = 1; k <= n; k++) {
                    if (a[i][k] && b[k][j]) {
                        m[i][j] = true;
                        
                        if (th.second[i][j] == 0) {
                            th.second[i][j] = k;
                        }
                    }
                }
            }
        }

        // Storing the through elements result in ths
        ths.push_back(th);

        // Storing the matrix result in adjs
        adjs.push_back({_s, m});
    }

    void initialize_task2() {
        // initialize G
        vector<vector<bool>> G(n + 1, vector<bool>(n + 1, false));
        for (int i = 1; i <= n; ++i) {
            for (auto& edge : adj[i]) {
                G[i][edge.first] = true;
            }
        }

        adjs.push_back({1, G});
    }

    // Calculate the exponents of the adacency matrices, forming a tree
    void matrix_power(int p) {
        int _s = p << 1, _r = 1;

        if (_s <= k) {
            _r = p;
        } else {
            while (_r + p <= k) _r <<= 1;
            _r >>= 1;
        }

        // search for the first element of the pair from the adjs vector to check if the matrix has actually been calculated
        bool power_calculated = std::any_of(adjs.begin(), adjs.end(), 
            [&p, &_r](const pair<int, vector<vector<bool>>>& pair) {
                return pair.first == p + _r;
            }
        );
        if (power_calculated) return;

        // get the left and right matrices, searching after the first element from the pair
        auto lmat = std::find_if(adjs.begin(), adjs.end(), [&p](const pair<int, vector<vector<bool>>>& pair) { return pair.first == p; });
        auto rmat = std::find_if(adjs.begin(), adjs.end(), [&_r](const pair<int, vector<vector<bool>>>& pair) { return pair.first == _r; });

        if (lmat == adjs.end() || rmat == adjs.end()) return;

        multiply(lmat->second, rmat->second, p + _r);
        th_components.push_back({p + _r, {p, _r}});

        int next = p + _r;
        if (next == k)  return;
        matrix_power(p + _r);
    }

    void merge_results() {
        if (rres == "-1" || lres == "-1") {
            result = "-1";
        }
    
        result = lres + rres;
    }

    bool node_is_final(int node) {
        return std::any_of(f.begin(), f.end(), [&node](const int& final_node) { return final_node == node; });
    }

    void rec_bfs(int node, int steps, vector<bool>& visited, string path, int &s_node) {
        if (steps >= min_bfs) return;

        if (node_is_final(node)) {
            min_bfs = steps;
            rres = path;
            bfs_src_node = s_node;
            return;
        }

        visited[node] = true;

        for (auto &edge : adj[node]) {
            if (!visited[edge.first]) {
                rec_bfs(edge.first, steps + 1, visited, path + edge.second, s_node);
            }
        }

        visited[node] = false;
    }

    void bfs() {
        // check if there is an adj matrix for pow k
        int loc_k = k;
        auto adj_k = std::find_if(adjs.begin(), adjs.end(), [&loc_k](const pair<int, vector<vector<bool>>>& pair) { return pair.first == loc_k; });
        if (adj_k == adjs.end()) {
            rres = "-1";
            return;
        }

        // extract the nodes from where to start from the adj matrix of pow k
        vector <int> s_nodes;
        for (int i = 1; i <= n; i++) {
            if (adj_k->second[s][i]) {
                s_nodes.push_back(i);
            }
        }

        // if there are no nodes from where to start, the result is -1
        if (s_nodes.empty()) {
            rres = "-1";
            return;
        }

        // if one node from s_nodes is final then let the rres be empty
        for (auto& node : s_nodes) {
            if (node_is_final(node)) {
                bfs_src_node = node;
                rres = "";
                return;
            }
        }

        bfs_src_node = s_nodes[0];
        min_bfs = INT_MAX;
        for (auto& node : s_nodes) {
            vector<bool> visited(n + 1, false);
            rec_bfs(node, 0, visited, "", node);
        }

        if (min_bfs == INT_MAX) {
            rres = "-1";
        }
    }

    /*
        * src - the source node
        * dest - the dest node
        * p - the power of the adj matrix for the current step
    */
    string adj_reconstruct(int src, int dest, int p) {
        // cout << src << ' ' << dest << ' ' << p << '\n';
        if (src == 0 || dest == 0) return "";

        if (p == 1) {
            // extract char
            char c = std::find_if(adj[src].begin(), adj[src].end(), [&dest](const pair<int, char>& pair) { return pair.first == dest; })->second;
            string ret(1, c);
            return ret;
        }

        // get the through matrix for the current pow
        auto th_p = std::find_if(ths.begin(), ths.end(), [&p](const pair<int, vector<vector<int>>>& pair) { return pair.first == p; });

        if (th_p == ths.end()) {
            return "";
        }

        int l_pow = std::find_if(th_components.begin(), th_components.end(), [&p](const pair<int, pair<int, int>>& pair) { return pair.first == p; })->second.first;
        int r_pow = std::find_if(th_components.begin(), th_components.end(), [&p](const pair<int, pair<int, int>>& pair) { return pair.first == p; })->second.second;
        // cout << src << ' ' << dest << ' ' << p << ' ' << l_pow << ' ' << r_pow << '\n';

        string l_path = adj_reconstruct(src, th_p->second[src][dest], l_pow);
        string r_path = adj_reconstruct(th_p->second[src][dest], dest, r_pow);
    
        return l_path + r_path;
    }

    void solve() override {
        initialize_task2();
        matrix_power(1);
        bfs();

        // if rres is "-1" it means we cannot get to a final node so we don't need to reconstruct the path
        if (rres != "-1") {
            lres = adj_reconstruct(s, bfs_src_node, k);
        }

        merge_results();
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
