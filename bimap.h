#include <iostream>

template<typename T, typename U>
struct bimap
{
private:

    typedef T left_t;
    typedef U right_t;
    struct left_iterator;
    struct right_iterator;
    struct node_base {
        node_base*   left[2];
        node_base*   right[2];
        node_base*   parent[2];
        node_base () {
            left[0] = left[1] = right[0] = right[1] = parent[0] = parent[1] = nullptr;
        }
        virtual ~node_base() {}
    protected:
        friend left_iterator;
        friend right_iterator;
        friend bimap;

        node_base* next(size_t ind) {
            node_base* v = this;
            if (v->right[ind] != nullptr) {
                v = v->right[ind];
                while (v->left[ind] != nullptr)
                    v = v->left[ind];
                return v;
            }
            node_base* u = v->parent[ind];
            while (u != nullptr && v == u->right[ind]) {
                v = u;
                u = u->parent[ind];
            }
            return u;
        }
        node_base* prev(size_t ind) {
            node_base* v = this;
            if (v->left[ind] != nullptr) {
                v = v->left[ind];
                while (v->right[ind] != nullptr)
                    v = v->right[ind];
                return v;
            }
            node_base* u = v->parent[ind];
            while (u != nullptr && v == u->left[ind]) {
                v = u;
                u = u->parent[ind];
            }
            return u;
        }
    };
    struct node : node_base {
        left_t left_data;
        right_t right_data;
        node(left_t left_val, right_t right_val) : left_data(left_val), right_data(right_val) {}
    };
    struct left_iterator
    {
        typedef std::ptrdiff_t difference_type;
        typedef T value_type;
        typedef T const * pointer;
        typedef T const & reference;
        typedef std::bidirectional_iterator_tag iterator_category;

        left_t const& operator*() const {
            return dynamic_cast<node*>(v)->left_data;
        }

        left_iterator& operator++() {
            v = v->next(0);
            return *this;
        }
        left_iterator operator++(int) {
            left_iterator res = *this;
            ++*this;
            return res;
        }

        left_iterator& operator--() {
            v = v->prev(0);
            return *this;
        }
        left_iterator operator--(int) {
            left_iterator res = *this;
            ++*this;
            return res;
        }

        right_iterator flip() const {
            return right_iterator(v);
        }

        friend bool operator== (const left_iterator& a, const left_iterator& b) {
            return a.v == b.v;
        }
        friend bool operator!= (const left_iterator& a, const left_iterator& b) {
            return a.v != b.v;
        }
    private:
        friend bimap;
        friend right_iterator;

        left_iterator(node_base* v) : v(v) {}
        node_base* v;
    };

    struct right_iterator
    {
        typedef std::ptrdiff_t difference_type;
        typedef U value_type;
        typedef U const * pointer;
        typedef U const & reference;
        typedef std::bidirectional_iterator_tag iterator_category;
        
        right_t const& operator*() const {
            return dynamic_cast<node*>(v)->right_data;
        }

        right_iterator& operator++() {
            v = v->next(1);
            return *this;
        }
        right_iterator operator++(int) {
            right_iterator res = *this;
            ++*this;
            return res;
        }

        right_iterator& operator--() {
            v = v->prev(1);
            return *this;
        }
        right_iterator operator--(int) {
            right_iterator res = *this;
            ++*this;
            return res;
        }

        left_iterator flip() const {
            return left_iterator(v);
        }

        friend bool operator== (const right_iterator& a, const right_iterator& b) {
            return a.v == b.v;
        }
        friend bool operator!= (const right_iterator& a, const right_iterator& b) {
            return a.v != b.v;
        }
    private:
        friend bimap;
        friend left_iterator;

        right_iterator(node_base* v) : v(v) {}

        node_base* v;
    };

public:
    typedef std::reverse_iterator<left_iterator> reverse_left_iterator;
    typedef std::reverse_iterator<right_iterator> reverse_right_iterator;

    bimap(const bimap&) = delete;
    bimap& operator=(const bimap&) = delete;

    bimap() {
        end_ = new node_base;
    }

    ~bimap() {
        while (begin_left() != end_left()) {
            erase(begin_left());
        }
        delete end_;
    }

    // Вставка пары (left, right), возвращает итератор на left.
    // Если такой left или такой right уже присутствуют в bimap, вставка не
    // производится и возвращается end_left().
    left_iterator insert(left_t const& left, right_t const& right) {
        if (find_left(left) != end_left() || find_right(right) != end_right())
            return end_left();
        node* newNode = new node(left, right);
        insert_left(newNode);
        insert_right(newNode);
        return left_iterator(newNode);
    }

    void erase(left_iterator it) {
        erase(it.v, 0);
        erase(it.v, 1);
        delete it.v;
    }
    void erase(right_iterator it) {
        erase(it.v, 0);
        erase(it.v, 1);
        delete it.v;
    }

    // Возвращает итератор по элементу. В случае если элемент не найден, возвращает
    // end_left()/end_right() соответственно.
    left_iterator  find_left (left_t  const& left)  const {
        node* x = dynamic_cast<node*>(end_->left[0]);
        while (x != nullptr && x->left_data != left) {
            if (left < x->left_data)
                x = dynamic_cast<node*>(x->left[0]);
            else
                x = dynamic_cast<node*>(x->right[0]);
        }
        return x == nullptr ? end_left() : left_iterator(x);
    }
    right_iterator find_right(right_t const& right) const {
        node* x = dynamic_cast<node*>(end_->left[1]);
        while (x != nullptr && x->right_data != right) {
            if (right < x->right_data)
                x = dynamic_cast<node*>(x->left[1]);
            else
                x = dynamic_cast<node*>(x->right[1]);
        }
        return x == nullptr ? end_right() : right_iterator(x);
    }

    // Возващает итератор на минимальный по величине left.
    left_iterator begin_left() const {
        node_base* res = end_;
        while (res->left[0] != nullptr)
            res = res->left[0];
        return left_iterator(res);
    }
    // Возващает итератор на следующий за последним по величине left.
    left_iterator end_left() const {
        return left_iterator(end_);
    }

    // Возващает итератор на минимальный по величине right.
    right_iterator begin_right() const {
        node_base* res = end_;
        while (res->left[1] != nullptr)
            res = res->left[1];
        return right_iterator(res);
    }
    // Возващает итератор на следующий за последним по величине right.
    right_iterator end_right() const {
        return right_iterator(end_);
    }
private:
    node_base* end_;

    void insert_left(node* v) {
        if (end_->left[0] == nullptr) {
            end_->left[0] = v;
            v->parent[0] = end_;
            return;
        }
        node* x = dynamic_cast<node*>(end_->left[0]);
        while (x != nullptr && x->left_data != v->left_data) {
            if (v->left_data > x->left_data) {
                if (x->right[0] != nullptr)
                    x = dynamic_cast<node*>(x->right[0]);
                else {
                    v->parent[0] = x;
                    x->right[0] = v;
                    break;
                }
            } else if (x->left_data > v->left_data) {
                if (x->left[0] != nullptr)
                    x = dynamic_cast<node*>(x->left[0]);
                else {
                    v->parent[0] = x;
                    x->left[0] = v;
                    break;
                }
            }
        }
    }

    void insert_right(node* v) {
        if (end_->left[1] == nullptr) {
            end_->left[1] = v;
            v->parent[1] = end_;
            return;
        }
        node* x = dynamic_cast<node*>(end_->left[1]);
        while (x != nullptr && x->right_data != v->right_data) {
            if (v->right_data > x->right_data) {
                if (x->right[1] != nullptr)
                    x = dynamic_cast<node*>(x->right[1]);
                else {
                    v->parent[1] = x;
                    x->right[1] = v;
                    break;
                }
            } else if (x->right_data > v->right_data) {
                if (x->left[1] != nullptr)
                    x = dynamic_cast<node*>(x->left[1]);
                else {
                    v->parent[1] = x;
                    x->left[1] = v;
                    break;
                }
            }
        }
    }

    void erase(node_base* v, int ind) {
        node_base* p = v->parent[ind];
        if (v->left[ind] == nullptr && v->right[ind] == nullptr) {
            if (p->left[ind] == v) { p->left[ind] = nullptr; }
            else { p->right[ind] = nullptr; }
        } else if (v->left[ind] == nullptr || v->right[ind] == nullptr) {
            if (v->left[ind] == nullptr) {
                if (p->left[ind] == v) { p->left[ind] = v->right[ind]; }
                else { p->right[ind] = v->right[ind]; }
                v->right[ind]->parent[ind] = p;
            } else {
                if (p->left[ind] == v) { p->left[ind] = v->left[ind]; }
                else { p->right[ind] = v->left[ind]; }
                v->left[ind]->parent[ind] = p;
            }
        } else {
            node_base* x = v->next(ind);
            if (x->parent[ind]->left[ind] == x) {
                x->parent[ind]->left[ind] = x->right[ind];
                if (x->right[ind] != nullptr)
                    x->right[ind]->parent[ind] = x->parent[ind];
            } else {
                x->parent[ind]->right[ind] = x->right[ind];
                if (x->right[ind] != nullptr)
                    x->right[ind]->parent[ind] = x->parent[ind];
            }
            x->left[ind] = v->left[ind];
            x->left[ind]->parent[ind] = x;
            x->right[ind] = v->right[ind];
            if (x->right[ind] != nullptr) { x->right[ind]->parent[ind] = x; }
            x->parent[ind] = v->parent[ind];
            if (x->parent[ind]->left[ind] == v) { x->parent[ind]->left[ind] = x; }
            else { x->parent[ind]->right[ind] = x; }
        }
    }
};
