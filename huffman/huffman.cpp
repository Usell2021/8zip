#include <iostream>

FILE *f;

struct Forest {
    int weight;
    int root;
    int symbol;
};

struct Tree {
    int left;
    int right;
    int parent;
    int symbol;
};

std::pair<int, int> getMinsIdx(Forest f[], int s) {
    Forest wm = {
            .weight = 0,
            .root = 0,
    };

    for (int i = 0; i < s; i++) {
        if (wm.weight < f[i].weight)
            wm = {
                    .weight = f[i].weight,
                    .root = i
            };
    }
    Forest a = wm, b = wm;

    for (int i = 0; i < s; i++) {
        if (a.weight > f[i].weight) {
            a = {
                    .weight = f[i].weight,
                    .root = i
            };
        }
    }

    for (int i = 0; i < s; i++) {
        if (f[i].weight <= b.weight && a.root != i) {
            b = {
                    .weight = f[i].weight,
                    .root = i
            };
        }
    }
    return {a.root, b.root};
}



int frequency[256];
Forest forest[256];
Tree tree[512];
int tree_size;
int forest_size;
std::pair<int, int> keys[256];

void getKeys(Tree t[], int p, int k, int l) {
    if (t[p].left == t[p].right) {
        keys[p].first = k;
        keys[p].second = l;
    } else if (t[p].left != -1) {
        getKeys(t, t[p].left, k * 10, l+1);
    }
    if (t[p].right != -1) {
        getKeys(t, t[p].right, k * 10 + 1, l+1);
    }
}

int main() {
    f = fopen("./input.txt", "rb");
    unsigned char ch;

    while (fscanf(f, "%c", &ch) != -1) {
        frequency[ch]++;
    }

    for (int i = 0; i < 256; i++) {
        auto item = frequency[i];
        if (item > 0) {
            forest[forest_size] = {
                    .weight = item,
                    .root = forest_size,
                    .symbol = i,
            };

            forest_size++;
        }
    }

    for (auto &item: tree) {
        item = {
                .left = -1,
                .right = -1,
                .parent = -1,
                .symbol = -1,
        };
    }

    tree_size = forest_size;
    while (forest_size > 1) {
        auto mins = getMinsIdx(forest, forest_size);
        auto fa = forest[mins.first], fb = forest[mins.second];
        tree_size++;

        int tr = tree_size - 1;
        Forest ft = {
                .weight = fa.weight + fb.weight,
                .root = tr
        };

        tree[fa.root] = {
                .left = tree[fa.root].left,
                .right = tree[fa.root].right,
                .parent = tr,
                .symbol = fa.symbol,
        };
        tree[fb.root] = {
                .left = tree[fb.root].left,
                .right = tree[fb.root].right,
                .parent = tr,
                .symbol = fb.symbol,
        };
        tree[tr] = {
                .left = fa.root,
                .right = fb.root,
                .parent = tree[tr].parent
        };


        if (mins.first == forest_size - 1) {
            forest[mins.second] = ft;
        } else if (mins.second == forest_size - 1) {
            forest[mins.first] = ft;
        } else {
            forest[mins.first] = ft;
            forest[mins.second] = forest[forest_size - 1];
        }

        forest_size--;
    }

    getKeys(tree, tree_size - 1, 0, 0);

    for (int i = 0; i < 256; i++) {
        auto item = keys[i];
        if (item.second != 0)
        std::cout << char(tree[i].symbol) << ' ' << item.first << ' ' << item.second << '\n';
    }

    f = fopen("./input.txt", "rb");
    while (fscanf(f, "%c", &ch) != -1) {
        frequency[ch]++;
    }


    std::cout << "norm";

}