#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stddef.h>
#include <algorithm>
#include <vector>
#include <functional>
#include <queue>
#define getchar getchar_unlocked
#define putchar putchar_unlocked
using namespace std;
typedef long long ll;
bool eof_flag;
bool rd(int *s)
{
    if (eof_flag)
        return 0;
    int k = 0, f = 1;
    char c = getchar();
    while (c != '-' && (c < '0' || c > '9'))
    {
        if (c == EOF)
        {
            eof_flag = 1;
            return 0;
        }
        c = getchar();
    }
    f = (c == '-') ? -1 : 1;
    k = (c == '-') ? 0 : (c ^ 48);
    c = getchar();
    while (c >= '0' && c <= '9')
        k = (k << 1) + (k << 3) + (c ^ 48), c = getchar();
    if (c == EOF)
        eof_flag = 1;
    (*s) = f > 0 ? k : -k;
    return 1;
}
void wr(ll x)
{
    if (x > 9)
        wr(x / 10);
    putchar(x % 10 + '0');
}
const ll INF = 1145141919810114514ll << 2;
// basic info and dimension
// use id when necessary
template <size_t K>
struct pt
{
    int d[K], id;
    pt<K>() = default;
};
template <size_t K>
struct point
{
    pt<K> d, mx, mn; // current, max and min coordinate
    int l, r;        // lchild & rchild
    ll v, sum;       // weight & sum
    point(pt<K> p = pt<K>(), ll _v = 0) { d = mx = mn = p, sum = v = _v, l = r = 0; }
    // 0 : manhattan 1 : euclidean
    inline ll dis(const point &o, const bool f = 1) const
    {
        ll ret = 0, dis = 0;
        for (int i = 0; i < K; ++i)
        {
            dis = abs(d.d[i] - o.d.d[i]);
            ret += f ? 1ll * dis * dis : 1ll * dis;
        }
        return ret;
    }
    // not real value
    inline ll dis_mn(const point &o, const bool f = 1) const
    {
        ll ret = 0, dis = 0;
        for (int i = 0; i < K; ++i)
        {
            dis = 0;
            if (o.d.d[i] < mn.d[i])
                dis = mn.d[i] - o.d.d[i];
            if (o.d.d[i] > mx.d[i])
                dis = o.d.d[i] - mx.d[i];
            ret += f ? 1ll * dis * dis : 1ll * dis;
        }
        return ret;
    }
    inline ll dis_mx(const point &o, const bool f = 1) const
    {
        ll ret = 0, dis = 0;
        for (int i = 0; i < K; ++i)
        {
            dis = max(1ll * abs(mn.d[i] - o.d.d[i]), 1ll * abs(mx.d[i] - o.d.d[i]));
            ret += f ? 1ll * dis * dis : 1ll * dis;
        }
        return ret;
    }
    inline bool operator==(const point &a) const
    {
        for (int i = 0; i < K; ++i)
            if (a.d.d[i] != this->d.d[i])
                return false;
        return true;
    }
    // all points from this sub-tree in this area
    inline bool in(const point &lower, const point &upper)
    {
        for (int i = 0; i < K; ++i)
            if (!(this->mn.d[i] >= lower.d.d[i] && this->mx.d[i] <= upper.d.d[i]))
                return false;
        return true;
    }
    // only it self in this area
    inline const bool inself(const point &lower, const point &upper) const
    {
        for (int i = 0; i < K; ++i)
            if (!(this->d.d[i] >= lower.d.d[i] && this->d.d[i] <= upper.d.d[i]))
                return false;
        return true;
    }
    // all points from this sub-tree are not in this area
    inline const bool out(const point &lower, const point &upper) const
    {
        for (int i = 0; i < K; ++i)
            if (this->mn.d[i] > upper.d.d[i] || this->mx.d[i] < lower.d.d[i])
                return true;
        return false;
    }
    void print() const
    {
        printf("d : (");
        for (int i = 0; i < K; ++i)
            printf("%d%c", d.d[i], (i < K - 1) ? ',' : ')');
        putchar('\n');

        printf("mx : (");
        for (int i = 0; i < K; ++i)
            printf("%d%c", mx.d[i], (i < K - 1) ? ',' : ')');
        putchar('\n');

        printf("mn : (");
        for (int i = 0; i < K; ++i)
            printf("%d%c", mn.d[i], (i < K - 1) ? ',' : ')');
        putchar('\n');

        printf("l : %d, r : %d\n", l, r);
        printf("v : %lld, sum : %lld\n", v, sum);
    }
};
// use for K-Nearest-Neighbor (KNN)
struct pt_info
{
    ll dis;
    int id;
    pt_info(ll _d = 0, int _id = 0) : dis(_d), id(_id) {}
};
bool _cmp_near(const pt_info &a, const pt_info &b)
{
    if (a.dis ^ b.dis)
        return a.dis < b.dis;
    else
        return a.id < b.id;
}
bool _cmp_far(const pt_info &a, const pt_info &b)
{
    if (a.dis ^ b.dis)
        return a.dis > b.dis;
    else
        return a.id < b.id;
}
struct pt_cmp_near
{
    bool operator()(const pt_info &a, const pt_info &b) const { return _cmp_near(a, b); }
};
struct pt_cmp_far
{
    bool operator()(const pt_info &a, const pt_info &b) const { return _cmp_far(a, b); }
};
template <size_t K>
struct KD_Tree_Static
{
    // assume points[0] is default
    vector<point<K>> points;
    int root;
    int n; // size of point

    inline void print() const
    {
        for (int i = 1; i <= n; ++i)
            printf("point %d : \n", i), points[i].print();
    }
    inline void update(int k)
    {
        int l = points[k].l, r = points[k].r;
        for (int i = 0; i < K; ++i)
        {
            points[k].mn.d[i] = points[k].mx.d[i] = points[k].d.d[i];
            if (l)
            {
                points[k].mn.d[i] = min(points[k].mn.d[i], points[l].mn.d[i]);
                points[k].mx.d[i] = max(points[k].mx.d[i], points[l].mx.d[i]);
            }
            if (r)
            {
                points[k].mn.d[i] = min(points[k].mn.d[i], points[r].mn.d[i]);
                points[k].mx.d[i] = max(points[k].mx.d[i], points[r].mx.d[i]);
            }
        }
        points[k].sum = points[k].v + points[l].sum + points[r].sum;
    }
    // construct : return root
    // cur_d : in 2D is bool, can be reconstructed to int
    KD_Tree_Static() { points.clear(), points.push_back(point<K>()); }
    // assume pts[0] is default
    KD_Tree_Static(const vector<point<K>> &pts)
    {
        points.resize(pts.size());
        n = pts.size() - 1;
        vector<point<K>> tmp = pts;
        function<int(int, int, int)> construct = [&](int l, int r, int cur_d)
        {
            if (l > r)
                return 0;
            int mi = (l + r) >> 1;
            function<bool(const point<K> &, const point<K> &)> cmp = [&](const point<K> &a, const point<K> &b)
            {
                return a.d.d[cur_d] < b.d.d[cur_d];
            };
            nth_element(tmp.begin() + l, tmp.begin() + mi, tmp.begin() + r + 1, cmp);
            points[mi] = tmp[mi];
            int nxt_d = (cur_d == K - 1) ? 0 : cur_d + 1;
            points[mi].l = construct(l, mi - 1, nxt_d); // use (cur_d + 1) % K when K > 2
            points[mi].r = construct(mi + 1, r, nxt_d);
            update(mi);
            return mi;
        };
        root = construct(1, n, 0);
    }
    // query from subtree with root points[k]
    inline ll _query(int k, const point<K> &lower, const point<K> &upper)
    {
        if (!k)
            return 0;
        ll ret = 0;
        if (points[k].in(lower, upper))
            return points[k].sum;
        if (points[k].out(lower, upper))
            return 0;
        if (points[k].inself(lower, upper))
            ret += points[k].v;
        ret += _query(points[k].l, lower, upper) + _query(points[k].r, lower, upper);
        return ret;
    }
    // query from [lower.d[0], upper.d[0]]x...x[lower.d[K-1], upper.d[K-1]]
    ll query(const point<K> &lower, const point<K> &upper)
    {
        return _query(root, lower, upper);
    }
    // query distance
    void _query_mn_dis(ll &cur_min_dis, int k, const point<K> &a, const bool f = 1)
    {
        ll tmp_dis = points[k].dis(a, f);
        // remove it when necessary
        if (!tmp_dis)
            tmp_dis = INF;
        cur_min_dis = min(cur_min_dis, tmp_dis);
        ll tmpl = points[k].l ? points[points[k].l].dis_mn(a, f) : INF;
        ll tmpr = points[k].r ? points[points[k].r].dis_mn(a, f) : INF;
        if (tmpl < tmpr)
        {
            if (tmpl < cur_min_dis)
                _query_mn_dis(cur_min_dis, points[k].l, a, f);
            if (tmpr < cur_min_dis)
                _query_mn_dis(cur_min_dis, points[k].r, a, f);
        }
        else
        {
            if (tmpr < cur_min_dis)
                _query_mn_dis(cur_min_dis, points[k].r, a, f);
            if (tmpl < cur_min_dis)
                _query_mn_dis(cur_min_dis, points[k].l, a, f);
        }
    }
    // 0 : manhattan 1 : euclidean
    ll query_min_dis(const point<K> &a, const bool f = 1)
    {
        ll ret = INF;
        _query_mn_dis(ret, root, a, f);
        return ret;
    }
    void _query_mx_dis(ll &cur_min_dis, int k, const point<K> &a, const bool f = 1)
    {
        ll tmp_dis = points[k].dis(a, f);
        // remove it when necessary
        if (!tmp_dis)
            tmp_dis = -INF;
        cur_min_dis = max(cur_min_dis, tmp_dis);
        ll tmpl = points[k].l ? points[points[k].l].dis_mx(a, f) : -1ll;
        ll tmpr = points[k].r ? points[points[k].r].dis_mx(a, f) : -1ll;
        if (tmpl > tmpr)
        {
            if (tmpl > cur_min_dis)
                _query_mx_dis(cur_min_dis, points[k].l, a, f);
            if (tmpr > cur_min_dis)
                _query_mx_dis(cur_min_dis, points[k].r, a, f);
        }
        else
        {
            if (tmpr > cur_min_dis)
                _query_mx_dis(cur_min_dis, points[k].r, a, f);
            if (tmpl > cur_min_dis)
                _query_mx_dis(cur_min_dis, points[k].l, a, f);
        }
    }
    // 0 : manhattan 1 : euclidean
    ll query_max_dis(const point<K> &a, const bool f = 1)
    {
        ll ret = -INF;
        _query_mx_dis(ret, root, a, f);
        return ret;
    }
};
vector<point<2>> points;
int n;
int main()
{
    while (rd(&n))
    {
        points.resize(n + 1);

        for (int i = 1; i <= n; ++i)
            rd(&points[i].d.d[0]), rd(&points[i].d.d[1]), points[i].d.id = i;

        KD_Tree_Static<2> kdt = KD_Tree_Static<2>(points);
        ll ans = -INF;

        for (int i = 1; i <= n; ++i)
            kdt._query_mx_dis(ans, kdt.root, points[i]); // wt(kdt.query_min(points[i]), '\n');

        wr(ans), putchar('\n');
    }
}