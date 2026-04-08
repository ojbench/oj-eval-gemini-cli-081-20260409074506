#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>

using namespace std;

struct Edge {
    int to;
    int cap;
    int flow;
    int rev;
};

vector<vector<Edge>> adj;

void add_edge(int u, int v, int cap) {
    adj[u].push_back({v, cap, 0, (int)adj[v].size()});
    adj[v].push_back({u, cap, 0, (int)adj[u].size() - 1});
}

int n, m;
vector<int> level;
vector<int> ptr;

bool bfs(int s, int t) {
    level.assign(n + 1, -1);
    level[s] = 0;
    queue<int> q;
    q.push(s);
    while (!q.empty()) {
        int v = q.front();
        q.pop();
        for (auto& edge : adj[v]) {
            if (edge.cap - edge.flow > 0 && level[edge.to] == -1) {
                level[edge.to] = level[v] + 1;
                q.push(edge.to);
            }
        }
    }
    return level[t] != -1;
}

int dfs(int v, int t, int pushed) {
    if (pushed == 0) return 0;
    if (v == t) return pushed;
    for (int& cid = ptr[v]; cid < adj[v].size(); ++cid) {
        auto& edge = adj[v][cid];
        int tr = edge.to;
        if (level[v] + 1 != level[tr] || edge.cap - edge.flow == 0) continue;
        int push = dfs(tr, t, min(pushed, edge.cap - edge.flow));
        if (push == 0) continue;
        edge.flow += push;
        adj[tr][edge.rev].flow -= push;
        return push;
    }
    return 0;
}

int dinic(int s, int t) {
    for (int i = 1; i <= n; ++i) {
        for (auto& edge : adj[i]) {
            edge.flow = 0;
        }
    }
    int flow = 0;
    while (bfs(s, t)) {
        ptr.assign(n + 1, 0);
        while (int pushed = dfs(s, t, 1e9)) {
            flow += pushed;
        }
    }
    return flow;
}

vector<bool> visited;
void dfs_cut(int v) {
    visited[v] = true;
    for (auto& edge : adj[v]) {
        if (edge.cap - edge.flow > 0 && !visited[edge.to]) {
            dfs_cut(edge.to);
        }
    }
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    
    if (!(cin >> n >> m)) return 0;
    
    adj.resize(n + 1);
    for (int i = 0; i < m; ++i) {
        int u, v;
        cin >> u >> v;
        add_edge(u, v, 1);
    }
    
    vector<int> p(n + 1, 1);
    vector<int> weight(n + 1, 0);
    
    for (int i = 2; i <= n; ++i) {
        int s = i, t = p[i];
        weight[i] = dinic(s, t);
        visited.assign(n + 1, false);
        dfs_cut(s);
        for (int j = i + 1; j <= n; ++j) {
            if (p[j] == t && visited[j]) {
                p[j] = s;
            }
        }
    }
    
    vector<vector<pair<int, int>>> tree(n + 1);
    for (int i = 2; i <= n; ++i) {
        tree[i].push_back({p[i], weight[i]});
        tree[p[i]].push_back({i, weight[i]});
    }
    
    long long total_flow = 0;
    for (int i = 1; i <= n; ++i) {
        vector<int> min_edge(n + 1, 1e9);
        queue<int> q;
        q.push(i);
        min_edge[i] = 1e9;
        vector<bool> vis(n + 1, false);
        vis[i] = true;
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            for (auto& edge : tree[u]) {
                int v = edge.first;
                int w = edge.second;
                if (!vis[v]) {
                    vis[v] = true;
                    min_edge[v] = min(min_edge[u], w);
                    q.push(v);
                    if (i < v) {
                        total_flow += min_edge[v];
                    }
                }
            }
        }
    }
    
    cout << total_flow << "\n";
    return 0;
}
