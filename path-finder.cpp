#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <vector>
#include <string>
#include <fstream>
#include <algorithm>

using namespace std;

const int timeUpperLimit = 168;

class Location {
public:
    int index;
    int timeCost;
    int timeRangeN;
    std::vector<pair<int, int> > timeRanges;

    int getEndTime(int startTime) {
        int res = -1;
        for (int i = 0; i < timeRangeN; ++i) {
            if (timeRanges[i].first <= startTime && timeRanges[i].second >= startTime + timeCost) {
                res = (res == -1) ? (startTime + timeCost) : min(res, startTime + timeCost);
            }
        }
        return res;
    }
};

class SingleEdge {
public:
    int edgeIndex, selfIndex;
    int startTime, endTime, timeCost, moneyCost;
};

class Edge {
public:
    int start, end;
    int methodN;
    vector<SingleEdge> methods;
    void printSingle(int singleIndex) {
        cout << "Go from " << start << " to " << end << endl;
        cout << "use method " << singleIndex << ": ";
        cout << "time cost = " << methods[singleIndex].timeCost << ", money cost = " << methods[singleIndex].moneyCost << endl;
    }
};

class Step {
public:
    int edgeIndex, singleIndex;
    int departTime, reachTime, finishTime;
};
class Path {
public:
    vector<Step> edges;
    int timeCost;
    int moneyCost;
    Path() {
        edges.clear();
        timeCost = moneyCost = -1;
    }
};
class DPState {
public:
    int moneyCost;
    int fromEdgeIndex, fromSingleIndex;
    int currT, currS, currU;
    int fromT, fromS, fromU;
    int departTime, reachTime, finishTime;
    DPState() {
        moneyCost = fromEdgeIndex = fromSingleIndex = -1;
        currT = currS = currU = -1;
        fromT = fromS = fromU = -1;
    }
};

class PathFinder {
public:
    int n, m;
    vector<Location> loc;
    vector<vector<Edge> > e;
    vector<Edge> edges;
    int edgeIndex;

    Location loadLocation(fstream &ifs) {
        Location res;

        ifs >> res.timeCost;

        ifs >> res.timeRangeN;
        res.timeRanges.clear();
        pair<int, int> timeRange;
        for (int i = 0; i < res.timeRangeN; ++i) {
            ifs >> timeRange.first >> timeRange.second;
            if (timeRange.first > timeRange.second) {
                cerr << "Error: (" << timeRange.first << ", " << timeRange.second << ") is not a valid time range: start time later than end." << endl;
            }
            if (timeRange.second >= timeUpperLimit) {
                cerr << "Error: (" << timeRange.first << ", " << timeRange.second << ") is not a valid time range: end time later than upper limit" << endl;
            }
            res.timeRanges.push_back(timeRange);
        }

        return res;
    }

    SingleEdge loadSingleEdge(fstream &ifs) {
        SingleEdge res;
        ifs >> res.startTime >> res.endTime >> res.timeCost >> res.moneyCost;
        return res;
    }

    Edge loadEdge(fstream &ifs) {
        Edge res;
        ifs >> res.start >> res.end;
        ifs >> res.methodN;
        res.methods.clear();
        for (int i = 0; i < res.methodN; ++i) {
            res.methods.push_back(loadSingleEdge(ifs));
            res.methods.back().edgeIndex = edgeIndex;
            res.methods.back().selfIndex = int(res.methods.size()) - 1;
        }
        return res;
    }

    PathFinder(string filename) {
        fstream ifs(filename);
        ifs >> n;

        cout << "n = " << n << endl;
        
        loc.clear();
        for (int i = 0; i < n; ++i) {
            loc.push_back(loadLocation(ifs));
        }

        e = vector<vector<Edge> >(n);
        edgeIndex = 0;
        ifs >> m;

        for (int i = 0; i < m; ++i) {
            Edge edge = loadEdge(ifs);
            edges.push_back(edge);
            e[edge.start].push_back(edge);

            ++edgeIndex;
        }
    }

    void doNothingUpdate(DPState &a, DPState b) {
        if (a.moneyCost == -1 || a.moneyCost > b.moneyCost) {
            int aTime = a.currT;
            a = b;
            a.currT = aTime;
        }
    }

    void update(DPState &a, DPState b, SingleEdge &fromEdge, int departTime, int reachTime, int finishTime) {
        if (a.moneyCost == -1 || b.moneyCost + fromEdge.moneyCost < a.moneyCost) {
            a.moneyCost = b.moneyCost + fromEdge.moneyCost;
            a.fromEdgeIndex = fromEdge.edgeIndex;
            a.fromSingleIndex = fromEdge.selfIndex;

            a.fromT = b.currT;
            a.fromS = b.currS;
            a.fromU = b.currU;
            a.departTime = departTime;
            a.reachTime = reachTime;
            a.finishTime = finishTime;
        }

    }

    pair<Path, Path> optimize() {
        cout << "begin optimization" << endl;
        Path moneyOptimizePath;
        Path timeOptimizePath;
        int numStates = (1 << n);
        int mask = numStates - 1;

        vector<vector<vector<DPState> > > dp(timeUpperLimit, vector<vector<DPState> >(numStates, vector<DPState>(n)));
        
        for (int t = 0; t < timeUpperLimit; ++t) {
            for (int s = 0; s < numStates; ++s) {
                for (int u = 0; u < n; ++u) {
                    dp[t][s][u].currT = t;
                    dp[t][s][u].currS = s;
                    dp[t][s][u].currU = u;
                }
            }
        }

        dp[0][1][0].moneyCost = 0;

        // -1 for not reached, otherwise the minimum money cost to (t, S, v)
        // initialize: (1, anyTime, 0) = 0
        // want: (numStates - 1, anyTime, 0)

        // update order: from low S to high S, from low t to high t
        // suppose the route satisfy (a, b) + (b, c) > (a, c), otherwise (a, c) should include (a, b) + (b, c)
        for (int t = 0; t < timeUpperLimit; ++t) {
            for (int s = 0; s < numStates; ++s) {
                for (int u = 0; u < n; ++u) {
                    // cout << s << ' ' << t << ' ' << u << endl;
                    if (dp[t][s][u].moneyCost == -1) {
                        continue;
                    }
                    // otherwise, update with current state
                    // 1. do not do anything

                    if (t + 1 < timeUpperLimit) {
                        doNothingUpdate(dp[t + 1][s][u], dp[t][s][u]);
                    }
                    // 2. try to go somewhere
                    
                    for (int ei = 0; ei < e[u].size(); ++ei) {
                        int v = e[u][ei].end;
                        if (v > 0 && ((1 << v) & s)) {
                            // v has been reached: no need to go to v again
                            continue;
                        }
                        // otherwise, consider go to v
                        // for each single edge
                        SingleEdge method;
                        for (int si = 0; si < e[u][ei].methodN; ++si) {
                            method = e[u][ei].methods[si];
                            if (t > method.endTime) {
                                // current method cannot be used
                                continue;
                            }
                            int departTime = max(t, method.startTime);
                            int reachTime = departTime + method.timeCost;
                            int finishTime = loc[v].getEndTime(reachTime);
                            if (finishTime == -1) {
                                continue;
                            }

                            int newState = s | (1 << v);
                            update(dp[finishTime][newState][v], dp[t][s][u], method, departTime, reachTime, finishTime);
                        }
                    }
                }
            }
        }

        // next step: find time optimized route
        // among those: find mony optimized(automatically done)
        bool timeOptFound = false, moneyOptFound = false;
        for (int t = 0; t < timeUpperLimit; ++t) {
            if (dp[t][mask][0].moneyCost != -1) {
                timeOptFound = true;
                timeOptimizePath = getPath(dp, t, mask, 0);
                break;
            }
        }
        int minimumMoneyCost = -1;
        int minimumMoneyCostT = -1;
        for (int t = 0; t < timeUpperLimit; ++t) {
            if (dp[t][mask][0].moneyCost != -1) {
                if (minimumMoneyCostT == -1 || dp[t][mask][0].moneyCost < minimumMoneyCost) {
                    minimumMoneyCostT = t;
                    minimumMoneyCost = dp[t][mask][0].moneyCost;
                } else {
                    
                }
            }
        }
        if (minimumMoneyCostT != -1) {
            moneyOptFound = true;
            moneyOptimizePath = getPath(dp, minimumMoneyCostT, mask, 0);
        }

        if (timeOptFound) {
            cout << "Time optimization path has been found." << endl;
        } else {
            cout << "Time optimization path cannot been found." << endl;
        }

        if (moneyOptFound) {
            cout << "Money optimization path has been found." << endl;
        } else {
            cout << "Money optimization path cannot been found." << endl;
        }
        return {timeOptimizePath, moneyOptimizePath};
    }

    Path getPath(vector<vector<vector<DPState> > > &dp, int t, int S, int u) {
        Path res;
        res.timeCost = t;
        res.moneyCost = dp[t][S][u].moneyCost;
        
        int tt = t, SS = S, uu = u;
        while (dp[tt][SS][uu].fromEdgeIndex != -1) {
            int newT = dp[tt][SS][uu].fromT;
            int newS = dp[tt][SS][uu].fromS;
            int newU = dp[tt][SS][uu].fromU;

            // res.edges.push_back({dp[tt][SS][uu].fromEdgeIndex, dp[tt][SS][uu].fromSingleIndex});

            Step step;
            step.edgeIndex = dp[tt][SS][uu].fromEdgeIndex;
            step.singleIndex = dp[tt][SS][uu].fromSingleIndex;
            step.departTime = dp[tt][SS][uu].departTime;
            step.reachTime = dp[tt][SS][uu].reachTime;
            step.finishTime = dp[tt][SS][uu].finishTime;
            res.edges.push_back(step);

            tt = newT; SS = newS; uu = newU;
        }
        reverse(res.edges.begin(), res.edges.end());
        return res;
    }

    void printPath(Path &p) {
        for (int i = 0; i < p.edges.size(); ++i) {
            int edgeIndex = p.edges[i].edgeIndex, singleIndex = p.edges[i].singleIndex;
            edges[edgeIndex].printSingle(singleIndex);
            cout << "on the way: [" << p.edges[i].departTime << ", " << p.edges[i].reachTime << "], finish at " << p.edges[i].finishTime << endl;
        }
        cout << "total time cost = " << p.timeCost << endl;
        cout << "total money cost = " << p.moneyCost << endl;
    }
};

int main(int argc, char **argv) {
    // cout << "Hello world!" << endl;
    string filename = "example.txt";
    if (argc > 1) {
        filename = string(argv[1]);
    }
    PathFinder pf(filename);
    auto paths = pf.optimize();
    cout << "Time optimal path: " << endl;
    cout << "---------------------------------------------------" << endl;
    pf.printPath(paths.first);
    cout << "---------------------------------------------------" << endl;

    cout << "Money optimal path: " << endl;
    cout << "---------------------------------------------------" << endl;
    pf.printPath(paths.second);
    cout << "---------------------------------------------------" << endl;
    return 0;
}