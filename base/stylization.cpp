/*
 * Filename: /home/hineven/prj/trstyle/routine.cpp
 * Path: /home/hineven/prj/trstyle
 * Created Date: Sunday, May 16th 2021, 10:46:52 pm
 * Author: Hineven
 * 
 * Copyright (c) 2021 QwQ Unlimited Company
 */

#include "stylization.hpp"
#include "data.hpp"
#include "heads.hpp"

namespace Stylization {
    constexpr int F_TRIANGLE_VALID = 1;
    struct ChainNode {
        short value;
        short next;
    };
    class Vector2Grad: public Vector2 {
    public:
        Vector2 grad {0};
        inline Vector2Grad () = default;
        inline explicit Vector2Grad (const Float & f): Vector2(f) {}
        inline Vector2Grad (Float a, Float b): Vector2(a, b) {assert(!isNan(x) && !isNan(y));}
        Vector2Grad & operator = (const Vector2 & t) {
            x = t.x;
            y = t.y;
            return *this;
        }
    };
    // memory pool of chains
    std::vector<ChainNode> chain_nodes;
    // Garabage recycling
    std::queue<int> mem_chain_nodes;
    // Trianglulation vertices
    std::vector<Vector2Grad> vertices;
    // Triangles adjacent to any vertices 
    std::vector<ChainNode> connections;
    inline bool isStatic (int i) {
        if(vertices[i].x == 0 && vertices[i].y == 0) return true ;
        if(vertices[i].x == 0 && vertices[i].y == Data::image_height) return true;
        if(vertices[i].x == Data::image_width && vertices[i].y == 0) return true;
        if(vertices[i].x == Data::image_width && vertices[i].y == Data::image_height) return true;
        return false;
    }
    inline bool isBorder (Vector2 u) {
        if(u.x == 0 || u.y == 0) return true;
        if(u.x == Data::image_width
        || u.y == Data::image_height) return true;
        return false;
    }
    inline bool isBorder (int i) {
        if(vertices[i].x == 0 || vertices[i].y == 0) return true;
        if(vertices[i].x == Data::image_width
        || vertices[i].y == Data::image_height) return true;
        return false;
    }
    inline void chainInsertElement (int position, short value) {
        if(connections[position].value < 0) {
            connections[position].value = value;
            return ;
        }
        int u;
        if(!mem_chain_nodes.empty()) {
            u = mem_chain_nodes.front();
            mem_chain_nodes.pop();
        } else {
            u = chain_nodes.size();
            chain_nodes.push_back({-1, -1});
        }
        assert(u <= 32767);
        chain_nodes[u] = connections[position];
        connections[position].next = u;
        connections[position].value = value;
    }

    inline void chainRemoveElement (int position, short value) {
        if(connections[position].value == value) {
            if(connections[position].next < 0) {
                connections[position].value = -1;
                return ;
            }
            mem_chain_nodes.push(connections[position].next);
            connections[position] = 
                chain_nodes[connections[position].next];
            return ;
        }
        int cnt = 1;
        ChainNode * pre = &connections[position];
        int cur = connections[position].next;
        while(cur >= 0) {
            cnt ++;
            if(chain_nodes[cur].value == value) {
                pre->next = chain_nodes[cur].next;
                mem_chain_nodes.push(cur);
                return;
            }
            pre = &chain_nodes[cur];
            cur = chain_nodes[cur].next;
        }
        // Report a bug.
        fatal("Ah sure I'm f**ked.");
        // *1 *2 *3 *4 *5 *6 *7 *8 *9 *10 *11 *12 *13 *14 *15 *16 *17 *18, a total of 18 bugs fixed.
    }
    inline void chainPurge (int position) {
        int cur = connections[position].next;
        while(cur >= 0) {
            mem_chain_nodes.push(cur);
            cur = chain_nodes[cur].next;
        }
        connections[position] = {-1, -1};
    }
    struct Triangle {
        // Indices of triangle vertices, sorted in anti-clockwise order spatially
        short v[3], t;
        inline Vector2 operator [] (int i) const {return vertices[v[i]];}
        inline Vector2 & operator [] (int i) {return vertices[v[i]];}
    };
    std::vector<Triangle> triangles;
    // Garbage recycling
    std::queue<int> mem_vertices;
    std::queue<int> mem_triangles;

    inline bool validTriangle (int tr) {
        return triangles[tr].t&F_TRIANGLE_VALID;
    }
    inline bool validVertice (int vc) {
        return vertices[vc].x >= 0;
    }

    inline int newVertice () {
        if(mem_vertices.empty()) {
            vertices.push_back({0, 0});
            connections.push_back({-1, -1});
            return vertices.size()-1;
        }
        int i = mem_vertices.front();
        mem_vertices.pop();
        connections[i].value = -1;
        connections[i].next = -1;
        assert(i <= 32767);
        return i;
    }
    inline int newTriangle () {
        if(mem_triangles.empty()) {
            triangles.push_back({-1, -1, -1, 0});
            return triangles.size()-1;
        }
        int i = mem_triangles.front();
        mem_triangles.pop();
        assert(i <= 32767);
        return i;
    }
    inline void eraseVertice (int index) {
        chainPurge(index);
        vertices[index] = Vector2Grad(-1, -1);
        mem_vertices.push(index);
    }
    inline void eraseTriangle (int index) {
        triangles[index].t &= ~F_TRIANGLE_VALID;
        mem_triangles.push(index);
    }

    inline int numTriangles () {
        return triangles.size()-mem_triangles.size();
    }
    inline int numVertices () {
        return vertices.size()-mem_vertices.size();
    }
    
    // Split a triangle by inserting a new vertice at the centroid of the triangle
    bool splitTriangle (int index) {
        int a = triangles[index].v[0];
        int b = triangles[index].v[1];
        int c = triangles[index].v[2];
        chainRemoveElement(a, index);
        chainRemoveElement(b, index);
        chainRemoveElement(c, index);
        int nv = newVertice();
        vertices[nv] = (vertices[a]+vertices[b]+vertices[c])*(1.0f/3);
        eraseTriangle(index);
        int x = newTriangle();
        int y = newTriangle();
        int z = newTriangle();
        triangles[x] = {(short)a, (short)b, (short)nv, F_TRIANGLE_VALID};
        triangles[y] = {(short)b, (short)c, (short)nv, F_TRIANGLE_VALID};
        triangles[z] = {(short)c, (short)a, (short)nv, F_TRIANGLE_VALID};
        chainInsertElement(a, x);
        chainInsertElement(a, z);
        chainInsertElement(b, x);
        chainInsertElement(b, y);
        chainInsertElement(c, y);
        chainInsertElement(c, z);
        chainInsertElement(nv, x);
        chainInsertElement(nv, y);
        chainInsertElement(nv, z);
        return true;
    }
    // Collapse a triangle in one of the 3 following patterns
    // 1. merging vertices connected by its shortest edge
    // 2. flip an edge
    // 3. merge a vertice onto the image border
    // Special case when collapsing a triangle which has an edge overlaps the
    // image border
    bool collapseTriangle (int index) {
        if(!validTriangle(index)) return false;
        int a = triangles[index].v[0];
        int b = triangles[index].v[1];
        int c = triangles[index].v[2];
        // 3. merge a vertice onto the image border
        if((isBorder(a) && isBorder(b))
        || (isBorder(b) && isBorder(c))
        || (isBorder(c) && isBorder(a))) {
            // Special handeling, collapse the vertice onto the border
            if(isBorder(b) && isBorder(c)) std::swap(a, c);
            if(isBorder(a) && isBorder(c)) std::swap(b, c);
            //printf("collapse:(%f, %f), (%f, %f)\b", vertices[a].x, vertices[a].y, vertices[b].x, vertices[b].y);
            chainRemoveElement(a, index);
            chainRemoveElement(b, index);
            chainRemoveElement(c, index);
            eraseTriangle(index);
            if(vertices[a].x == 0 && vertices[b].x == 0) 
                vertices[c].x = 0;
            else if(vertices[a].x == Data::image_width
                 && vertices[b].x == Data::image_width)
                vertices[c].x = Data::image_width;
            else if(vertices[a].y == 0 && vertices[b].y == 0)
                vertices[c].y = 0;
            else vertices[c].y = Data::image_height;
            // It is possible for some vertice to be "totally dead" here
            // We need to get rid of them.
            if(connections[a].value == -1) eraseVertice(a);
            if(connections[b].value == -1) eraseVertice(b);
            if(connections[c].value == -1) eraseVertice(c);
            return true;
        }
        Float l0, l1, l2;
        l0 = sqrt((vertices[a]-vertices[b]).lengthSquared());
        l1 = sqrt((vertices[b]-vertices[c]).lengthSquared());
        l2 = sqrt((vertices[c]-vertices[a]).lengthSquared());
        // 2.merge a vertice onto an edge
        /*
        Float lmn = std::min(std::min(l0, l1), l2);
        Float lmx = std::max(std::max(l0, l1), l2);
        if((Data::collapse_edge_threshold < lmn
        || Data::collapse_edge_r_threshold*lmx < lmn)) {
            puts("Special collapse");
            
            if(l1 > l0 && l1 > l2) std::swap(a, b), std::swap(b, c); // b c a, keep counter-clokcwise
            if(l2 > l0 && l2 > l1) std::swap(a, c), std::swap(b, c); // c a b
            if(isBorder(c)) goto regret;
            // Map vertice c onto segment a-b
            vertices[c] =
                vertices[a]+(vertices[b]-vertices[a]).normalized()
              * dot(vertices[b]-vertices[a], vertices[c]-vertices[a]);
            chainRemoveElement(a, index);
            chainRemoveElement(b, index);
            chainRemoveElement(c, index);
            eraseTriangle(index);
            std::function<bool(int)> checkAndModify = [&a, &b, &c] (int i) {
                int C = 
                   (triangles[i].v[0] == a)+(triangles[i].v[0] == b)
                 + (triangles[i].v[1] == a)+(triangles[i].v[1] == b)
                 + (triangles[i].v[2] == a)+(triangles[i].v[2] == b);
                if(C < 2) return false;
                if(C != 2) fatal("WTF");
                int V;
                if(triangles[i].v[0] != a && triangles[i].v[0] != b)
                    V = triangles[i].v[0];
                else if(triangles[i].v[1] != a && triangles[i].v[1] != b)
                    V = triangles[i].v[1];
                else if(triangles[i].v[2] != a && triangles[i].v[2] != b)
                    V = triangles[i].v[2];
                else fatal("I'm f**ked again. And again.");
                // Find the triangle sharing the egde a-b: b-a-V
                // Split it to b-c-v and c-a-v
                chainRemoveElement(a, i);
                chainRemoveElement(b, i);
                chainRemoveElement(V, i);
                eraseTriangle(i);
                int X = newTriangle(), Y = newTriangle();
                triangles[X] = {(short)b, (short)c, (short)V, F_TRIANGLE_VALID};
                triangles[Y] = {(short)c, (short)a, (short)V, F_TRIANGLE_VALID};
                chainInsertElement(b, X);
                chainInsertElement(c, X);
                chainInsertElement(V, X);
                chainInsertElement(c, Y);
                chainInsertElement(a, Y);
                chainInsertElement(V, Y);
                return true;
            };
            int cur = connections[a].next;
            if(connections[a].value >= 0)
                if(checkAndModify(connections[a].value)) return true;
            while(cur >= 0) {
                if(checkAndModify(chain_nodes[cur].value))
                    return true;
                cur = chain_nodes[cur].next;
            }
            fatal("Yeah I'm f**ked again.");
        }
        regret:;*/
        // 1.merge two vertices
        std::function<void(int, int, int)> modify = [] (int tr, int x, int y) {
            if(triangles[tr].v[0] == y) triangles[tr].v[0] = x;
            if(triangles[tr].v[1] == y) triangles[tr].v[1] = x;
            if(triangles[tr].v[2] == y) triangles[tr].v[2] = x;
            int cnt = (int)(triangles[tr].v[0] == x)
                    + (triangles[tr].v[1] == x)
                    + (triangles[tr].v[2] == x);
            if(cnt >= 2) {
                chainRemoveElement(x, tr);
                int v0, v1, v2;
                v0 = triangles[tr].v[0];
                v1 = triangles[tr].v[1];
                v2 = triangles[tr].v[2];
                if(v0 != x) {
                    chainRemoveElement(v0, tr);
                    if(connections[v0].value < 0)
                        eraseVertice(v0);
                }
                if(v1 != x) {
                    chainRemoveElement(v1, tr);
                    if(connections[v1].value < 0)
                        eraseVertice(v1);
                }
                if(v2 != x) {
                    chainRemoveElement(v2, tr);
                    if(connections[v2].value < 0)
                        eraseVertice(v2);
                }
                eraseTriangle(tr);
            } else chainInsertElement(x, tr);
        };
        std::function<void(int, int)> operate = [&modify] (int x, int y) {
            if(isBorder(y)) std::swap(x, y);
            assert(!isBorder(y));
            int cur = connections[y].next;
            int tr = connections[y].value;
            // merge b to a
            modify(tr, x, y);
            while(cur >= 0) {
                tr = chain_nodes[cur].value;
                modify(tr, x, y);
                cur = chain_nodes[cur].next;
            }
            if(connections[x].value < 0) 
                eraseVertice(x);
            eraseVertice(y);
        };

        if(l0 <= l1 && l0 <= l2) operate(a, b);
        else if(l1 <= l0 && l1 <= l2) operate(b, c);
        else operate(c, a);

        return true;
    }

    // u0 -> v0 -> u1 -> v1
    void getFlipEdgeVertices (int X, int Y, int&u0, int&u1, int&v0, int &v1) {
        u0 = u1 = v0 = v1 = -1;
        if(triangles[X].v[0] != triangles[Y].v[0]
        && triangles[X].v[0] != triangles[Y].v[1]
        && triangles[X].v[0] != triangles[Y].v[2])
            u0 = triangles[X].v[0],
            v0 = triangles[X].v[1],
            v1 = triangles[X].v[2];
        if(triangles[X].v[1] != triangles[Y].v[0]
        && triangles[X].v[1] != triangles[Y].v[1]
        && triangles[X].v[1] != triangles[Y].v[2])
            u0 = triangles[X].v[1],
            v0 = triangles[X].v[2],
            v1 = triangles[X].v[0];
        if(triangles[X].v[2] != triangles[Y].v[0]
        && triangles[X].v[2] != triangles[Y].v[1]
        && triangles[X].v[2] != triangles[Y].v[2])
            u0 = triangles[X].v[2],
            v0 = triangles[X].v[0],
            v1 = triangles[X].v[1];
        if(u0 == -1) {
            // Triangles overlapped (case caused by collapsing edges)
            // Do nothing
            return ;
        }
        if(triangles[Y].v[0] != v0 && triangles[Y].v[0] != v1)
            u1 = triangles[Y].v[0];
        if(triangles[Y].v[1] != v0 && triangles[Y].v[1] != v1)
            u1 = triangles[Y].v[1];
        if(triangles[Y].v[2] != v0 && triangles[Y].v[2] != v1)
            u1 = triangles[Y].v[2];
    }

    // Flip an egde in current triangulation
    bool flipEdge (int X, int Y) {
        int u0, u1, v0, v1;
        getFlipEdgeVertices(X, Y, u0, u1, v0, v1);
        if(u0 < 0) return false;
        chainRemoveElement(u0, X);
        chainRemoveElement(v0, X);
        chainRemoveElement(v0, Y);
        chainRemoveElement(v1, X);
        chainRemoveElement(v1, Y);
        chainRemoveElement(u1, Y);
        eraseTriangle(X);
        eraseTriangle(Y);
        X = newTriangle();
        Y = newTriangle();
        triangles[X] = {(short)u0, (short)v0, (short)u1, F_TRIANGLE_VALID};
        triangles[Y] = {(short)u1, (short)v1, (short)u0, F_TRIANGLE_VALID};
        chainInsertElement(u0, X);
        chainInsertElement(u0, Y);
        chainInsertElement(u1, X);
        chainInsertElement(u1, Y);
        chainInsertElement(v0, X);
        chainInsertElement(v1, Y);
        return true;
    }

    Float verticeAttached (int index, Vector2 pos) {
        if(isBorder(pos)) return 0;
        std::function<Float(int)> func = [&index, &pos] (int tr) {
            Float ret = 0;
            if(triangles[tr].v[0] != index) 
                ret += (pos-triangles[tr][0]).lengthSquared();
            if(triangles[tr].v[1] != index) 
                ret += (pos-triangles[tr][1]).lengthSquared();
            if(triangles[tr].v[2] != index) 
                ret += (pos-triangles[tr][2]).lengthSquared();
            return ret;
        };
        // We assume that vertices adjancent to the vertice
        // are on a convex hull. Refer to the original paper
        // for more detailed infomation.
        Float ret = 0;
        int cur = connections[index].next;
        if(connections[index].value > 0)
            ret += func(connections[index].value);
        while(cur >= 0) {
            ret += func(chain_nodes[cur].value);
            cur = chain_nodes[cur].next;
        }
        return ret;
    }

    // Buffer should be thread local if this function is to be
    // called in parallel in multiple cores.
    Float verticeEnergy (int index, Vector2 pos, Float * buffer) {
        Float ret = verticeAttached(index, pos);
        Float sum = 0;
        int cur = connections[index].next;
        Vector2 x, y, z;
        int t = connections[index].value;
        x = triangles[t].v[0] == index
            ? pos : triangles[t][0];
        y = triangles[t].v[1] == index
            ? pos : triangles[t][1];
        z = triangles[t].v[2] == index 
            ? pos : triangles[t][2];
        sum += energy(x, y, z, false, buffer);
        while(cur >= 0) {
            t = chain_nodes[cur].value;
            x = triangles[t].v[0] == index
                ? pos : triangles[t][0];
            y = triangles[t].v[1] == index
                ? pos : triangles[t][1];
            z = triangles[t].v[2] == index 
                ? pos : triangles[t][2];
            sum += energy(x, y, z, false, buffer);
            cur = chain_nodes[cur].next;
        }
        return Data::lambda*ret+sum/3;
    }

    static pthread_mutex_t _fetch_workload;
    static int _interval_top;
    static int _interval_stop;
    static void _intervalFetchWorkload (int & l, int & r) {
        pthread_mutex_lock(&_fetch_workload);
        l = _interval_top;
        _interval_top = 
            std::min(_interval_top+64, _interval_stop);
        r = _interval_top-1;
        pthread_mutex_unlock(&_fetch_workload);
    }
    static void _verticeGradientsWorker (Vector2 * grad) {
        Float * buffer_memory = new Float[
            Data::image_height*Data::image_width*Data::image_nchan
        ];
        int L, R;
        while(true) {
            _intervalFetchWorkload(L, R);
            if(L > R) break ;
            for(int i = L; i<=R; i++) {
                if(!validVertice(i)) continue ;
                Vector2 dx0, dx1;
                dx0 = Data::clampInside(vertices[i] - Data::delta*Vector2(1, 0));
                dx1 = Data::clampInside(vertices[i] + Data::delta*Vector2(1, 0));
                Float ex0 = verticeEnergy(i, dx0, buffer_memory);
                Float ex1 = verticeEnergy(i, dx1, buffer_memory);
                Vector2 dy0, dy1;
                dy0 = Data::clampInside(vertices[i] - Data::delta*Vector2(0, 1));
                dy1 = Data::clampInside(vertices[i] + Data::delta*Vector2(0, 1));
                Float ey0 = verticeEnergy(i, dy0, buffer_memory);
                Float ey1 = verticeEnergy(i, dy1, buffer_memory);
                grad[i] = {(ex1-ex0)/(dx1.x-dx0.x), (ey1-ey0)/(dy1.y-dy0.y)};
            }
        }
        delete [] buffer_memory;
    }
    // Calculate vertice gradients
    void verticeGradients (Vector2 * grad) {
        _interval_top = 0;
        _interval_stop = vertices.size();
        std::thread * threads[Data::ncores];
        for(int i = 0; i<Data::ncores; i++) 
            threads[i] = new std::thread(_verticeGradientsWorker, grad);
        for(int i = 0; i<Data::ncores; i++) {
            threads[i]->join();
            delete threads[i];
        }
    }
    

    static void _maintainTriangulationWorker (Float * energies) {
        int L, R;
        while(true) {
            _intervalFetchWorkload(L, R);
            if(L > R) return ;
            for(int i = L; i<=R; i++) {
                if(!validTriangle(i)) continue ;
                energies[i] = energy(
                    triangles[i][0],
                    triangles[i][1],
                    triangles[i][2],
                    true
                );
            }
        }
    }
    static void _flipEdgesWorker (
        const std::vector<std::pair<int, int> > & pairs,
        Float * energies, bool * ifflip
    ) {
        Float * buffer_memory = new Float[
            Data::image_height*Data::image_width*Data::image_nchan
        ];
        int L, R;
        while(true) {
            _intervalFetchWorkload(L, R);
            if(L > R) break ;
            for(int i = L; i<=R; i++) {
                int X = pairs[i].first;
                int Y = pairs[i].second;
                int v0, v1, u0, u1;
                getFlipEdgeVertices(X, Y, u0, u1, v0, v1);
                if(u0 < 0) continue ;
                Float S = (energies[X]+energies[Y]) + Data::lambda*(vertices[v0]-vertices[v1]).lengthSquared();
                Float T = (energy(vertices[u0], vertices[u1], vertices[v0], false, buffer_memory)
                         + energy(vertices[u0], vertices[u1], vertices[v1], false, buffer_memory))
                         + Data::lambda*(vertices[u0]-vertices[u1]).lengthSquared();
                /*
                Float s1, s2;
                Float t1, t2;
                s1 = abs(cross(vertices[v0]-vertices[u0], vertices[u1]-vertices[u0]));
                s2 = abs(cross(vertices[v1]-vertices[u0], vertices[u1]-vertices[u0]));
                t1 = abs(cross(vertices[v0]-vertices[u0], vertices[v1]-vertices[u0]));
                t2 = abs(cross(vertices[v0]-vertices[u1], vertices[v1]-vertices[u1]));
                T += s1*s2;
                S += t1*t2;
                */
                if(T*(1.0f+Data::flip_threshould) < S) ifflip[i] = true;
                else ifflip[i] = false;
            }
        }
        delete [] buffer_memory;
    }

    constexpr int F_OPER_SPLIT = 1;
    constexpr int F_OPER_COLLAPSE = 2;
    constexpr int F_OPER_FLIP = 3;
    Float _cur_energy = Infinity;
    // Collapse triangles with too small area and split triangles
    // with too big energy density, flip edges as well as render
    // a result
    // if oper_type == 0, render the result only.
    bool maintainTriangulation (int oper_type) {
        bool ret = false;
        _interval_top = 0;
        _interval_stop = triangles.size();
        std::thread * threads[Data::ncores];
        Float * energies = new Float[triangles.size()];
        for(int i = 0; i<Data::ncores; i++) 
            threads[i] = new std::thread(_maintainTriangulationWorker, energies);
        for(int i = 0; i<Data::ncores; i++) {
            threads[i]->join();
            delete threads[i];
        }
        _cur_energy = 0;
        for(int i = 0; i<(int)triangles.size(); i++) {
            if(!validTriangle(i)) continue ;
            _cur_energy += energies[i];
        }
        for(int i = 0; i<(int)vertices.size(); i++) {
            if(!validVertice(i)) continue ;
            _cur_energy += Data::lambda*verticeAttached(i, vertices[i]);   
        }
        // Flip 
        if(oper_type == 3) {
            std::function<int(int, int)> pack = [] (int a, int b) {
                if(a > b) std::swap(a, b);
                return a*32768+b;
            };
            std::map<int, int> mp;
            std::vector<std::pair<int, int> > vpr;
            for(int i = 0; i<(int)triangles.size(); i++) {
                if(!validTriangle(i)) continue ;
                int a = triangles[i].v[0];
                int b = triangles[i].v[1];
                int c = triangles[i].v[2];
                auto it = mp.find(pack(a, b));
                if(it != mp.end()) vpr.push_back(std::make_pair(it->second, i));
                else mp[pack(a, b)] = i;
                it = mp.find(pack(b, c));
                if(it != mp.end()) vpr.push_back(std::make_pair(it->second, i));
                else mp[pack(b, c)] = i;
                it = mp.find(pack(c, a));
                if(it != mp.end()) vpr.push_back(std::make_pair(it->second, i));
                else mp[pack(c, a)] = i;
            }
            mp.clear();
            bool * ifflip = new bool[vpr.size()];
            _interval_top = 0;
            _interval_stop = vpr.size();
            std::thread * threads[Data::ncores];
            for(int i = 0; i<Data::ncores; i++) 
                threads[i] = new std::thread(_flipEdgesWorker, vpr, energies, ifflip);
            for(int i = 0; i<Data::ncores; i++) {
                threads[i]->join();
                delete threads[i];
            }
            bool * flipped = new bool[triangles.size()];
            memset(flipped, 0, triangles.size());
            for(int i = 0; i<(int)vpr.size(); i++) {
                if(ifflip[i] && !flipped[vpr[i].first] && !flipped[vpr[i].second]) {
                    flipped[vpr[i].first] = flipped[vpr[i].second] = true;
                    ret |= flipEdge (vpr[i].first, vpr[i].second);
                }
            }
            delete [] flipped;
            delete [] ifflip;
        } else if(oper_type == 1 || oper_type == 2) { // Split or Collapse
            bool fir = true;
            int minpos = -1;
            Float minv = Infinity;
            std::vector<std::pair<Float, int> > split_pos;
            for(int i = 0; i<(int)triangles.size(); i++) {
                if(!validTriangle(i)) continue ;
                Float area = cross(
                    triangles[i][1]-triangles[i][0],
                    triangles[i][2]-triangles[i][0]
                )/2;
                if(area < 0 && fir) 
                    puts("Area of a triangle goes negative!"), fir = false;
                if(area < Data::collapse_area_threshold) {
                    if(area < minv) minv = area, minpos = i;
                } else if(
                    energies[i]/area > Data::split_density_threshold
                 && energies[i] > Data::split_energy_lower_threshold
                 && area > Data::collapse_area_threshold*2
                ) split_pos.push_back(
                    std::make_pair(
                        energies[i]/area+Data::split_balance_factor*energies[i],
                        i
                    )
                );
            }
            if(oper_type == 1) {
                sort(split_pos.begin(), split_pos.end());
                int t = (int)split_pos.size()-Data::split_number+1;
                for(int i = (int)split_pos.size()-1; i>=std::max(0, t); i--) {
                    if(numTriangles() < Data::max_triangles-1)
                        ret |= splitTriangle(split_pos[i].second);
                }
            } else if(minpos != -1) ret |= collapseTriangle(minpos);
        }
        delete [] energies;
        return ret;
    }

    // Initialize the triangulation.
    void initializeWith (const Vector2 * verts, int nverts, const Triangle * trigs, int ntriangles) {
        pthread_mutex_init(&_fetch_workload, NULL);
        for(int i = 0; i<nverts; i++) {
            int vid = newVertice();
            // vid should equal to i
            assert(vid == i);
            vertices[vid] = verts[i];
            connections.push_back({-1, -1});
        }
        for(int i = 0; i<ntriangles; i++) {
            int tid = newTriangle();
            triangles[tid] = trigs[i];
            triangles[tid].t |= F_TRIANGLE_VALID;
            chainInsertElement(triangles[i].v[0], i);
            chainInsertElement(triangles[i].v[1], i);
            chainInsertElement(triangles[i].v[2], i);
        }
    }

    void initializeSimple () {
        Vector2 verts[5];
        verts[0] = {0, 0};
        verts[1] = {(Float)Data::image_width, 0};
        verts[2] = {(Float)Data::image_width, (Float)Data::image_height};
        verts[3] = {0, (Float)Data::image_height};
        verts[4] = verts[2]/2;
        Triangle trigs[4];
        trigs[0].v[0] = 0;
        trigs[0].v[1] = 1;
        trigs[0].v[2] = 4;
        trigs[1].v[0] = 1;
        trigs[1].v[1] = 2;
        trigs[1].v[2] = 4;
        trigs[2].v[0] = 2;
        trigs[2].v[1] = 3;
        trigs[2].v[2] = 4;
        trigs[3].v[0] = 3;
        trigs[3].v[1] = 0;
        trigs[3].v[2] = 4;
        initializeWith(verts, 5, trigs, 4);
    }

    bool shouldAdvance () {
        static Float best = Infinity;
        static int count = 0;
        if(_cur_energy < best*(1-epsilon))
            best = _cur_energy, count = 0;
        else count ++;
        if(count >= Data::split_stable) {
            count = 0;
            return true;
        }
        return false;
    }

    void iterate () {
        static int n_iters = 0;
        
        Vector2 * grad = new Vector2[vertices.size()];
        verticeGradients(grad);
        for(int i = 0; i<(int)vertices.size(); i++) {
            if(!validVertice(i)) continue ;
            if(isStatic(i)) continue ;
            vertices[i].grad = 
                Data::learning_lambda*vertices[i].grad + (1-Data::learning_lambda)*(grad[i]*grad[i]);
            Float sqx, sqy;
            sqx = sqrt(vertices[i].grad.x+epsilon);
            sqy = sqrt(vertices[i].grad.y+epsilon);

            Vector2 det = ((Data::h)*Vector2(1.0f/sqx, 1.0f/sqy))*grad[i];
            Float l = sqrt(det.lengthSquared());
            if(l > Data::smax) det *= Data::smax/l;
            if(isBorder(i)) {
                if(vertices[i].x == 0 || vertices[i].x == Data::image_width)
                    det.x = 0;
                else det.y = 0;
            }
            if(isBorder(i)) vertices[i] = Data::clamp(vertices[i]-det);
            else // Only through collapsing can a vertice become a border vertice
                vertices[i] = Data::clampInside(vertices[i]-det);
        }
        delete [] grad;
        bool f = false;
        if(n_iters <= 1 || shouldAdvance()) 
            maintainTriangulation(F_OPER_SPLIT), f = true;
        if(n_iters%Data::collapse_interval == 0) 
            maintainTriangulation(F_OPER_COLLAPSE), f = true;
        if(n_iters%Data::flip_interval == 0) 
            maintainTriangulation(F_OPER_FLIP), f = true;
        if(!f) maintainTriangulation(0);
        printf("verts:%d, trigs:%d\n", 
            (int)vertices.size()-(int)mem_vertices.size(),
            (int)triangles.size()-(int)mem_triangles.size()
        );
        n_iters ++;
    }

    void clear () {
        pthread_mutex_destroy(&_fetch_workload);
        chain_nodes.clear();
        while(!mem_chain_nodes.empty())
            mem_chain_nodes.pop();
        vertices.clear();
        connections.clear();
        triangles.clear();
        while(!mem_vertices.empty())
            mem_vertices.pop();
        while(!mem_triangles.empty())
            mem_triangles.pop();
    }
    
}