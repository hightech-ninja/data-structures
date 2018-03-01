#include <cassert>
#include <vector>

template<typename T>
struct list {
public:
    struct const_iterator;
    struct iterator;
private:
    struct node_base {
        iterator* my_first_iterator;
        const_iterator* my_first_const_iterator;
        node_base *l, *r;
        node_base() : my_first_iterator(nullptr), my_first_const_iterator(nullptr), l(nullptr), r(nullptr) {}
        node_base(node_base* l, node_base* r) : l(l), r(r) {}
        virtual ~node_base() {
            while (my_first_iterator != nullptr) {
                my_first_iterator->unlink(true);
            }
            while (my_first_const_iterator != nullptr) {
                my_first_const_iterator->unlink(true);
            }
        }
    };
    struct node : node_base {
        T val;
        node(T const& val) : val(val) {}
    };
public:
    struct iterator {
    public:
        typedef std::ptrdiff_t difference_type;
        typedef T value_type;
        typedef T const * pointer;
        typedef T const & reference;
        typedef std::bidirectional_iterator_tag iterator_category;

        iterator& operator++() {
            assert(valid && dynamic_cast<node*>(v) != nullptr);
            unlink();
            v = v->r;
            link();
            return *this;
        }
        iterator& operator--() {
            assert(valid && dynamic_cast<node*>(v->l) != nullptr);
            unlink();
            v = v->l;
            link();
            return *this;
        }
        iterator operator++(int) {
            iterator res = *this;
            ++*this;
            return res;
        }
        iterator operator--(int) {
            iterator res = *this;
            --*this;
            return res;
        }
        T& operator*() const {
            assert(valid && dynamic_cast<node*>(v) != nullptr);
            return dynamic_cast<node*>(v)->val;
        }
        friend bool operator==(const iterator& a, const iterator& b) {
            assert(a.valid && b.valid);
            return a.v == b.v;
        }
        friend bool operator!=(const iterator& a, const iterator& b) {
            assert(a.valid && b.valid);
            return a.v != b.v;
        }

        ~iterator() {
            unlink();
        }
        iterator() : v(nullptr), valid(false), parent(nullptr), next_iterator(nullptr) {}
        iterator(const iterator& it) : v(it.v), valid(it.valid), parent(it.parent) {
            if (valid)
                link();
        }
        iterator& operator=(const iterator& it) {
            if (this != &it) {
                unlink();
                v = it.v;
                valid = it.valid;
                parent = it.parent;
                next_iterator = nullptr;
                if (valid)
                    link();
            }
            return *this;
        }
    public:
        friend list;
        friend node_base;
        friend const_iterator;

        node_base* v;
        bool valid;
        list<T>* parent;
        iterator* next_iterator;

        iterator(node_base* v, list<T>* parent) : v(v), valid(true), parent(parent), next_iterator(nullptr) {
            link();
        }

        void unlink(bool with_invalidate = false) {
            if (!v)
                return;
            if (v->my_first_iterator == this) {
                v->my_first_iterator = v->my_first_iterator->next_iterator;
            }
            else {
                iterator *it = v->my_first_iterator;
                for (; it->next_iterator != this; it = it->next_iterator) {}
                it->next_iterator = it->next_iterator->next_iterator;
            }
            next_iterator = nullptr;
            if (with_invalidate)
                invalid();
        }
        void link() {
            if (v->my_first_iterator == nullptr)
                v->my_first_iterator = this;
            else {
                next_iterator = v->my_first_iterator;
                v->my_first_iterator = this;
            }
        }
        void invalid() {
            v = nullptr;
            valid = false;
            parent = nullptr;
        }
    };
    struct const_iterator {
    public:
        typedef std::ptrdiff_t difference_type;
        typedef T value_type;
        typedef T const * pointer;
        typedef T const & reference;
        typedef std::bidirectional_iterator_tag iterator_category;

        const_iterator& operator++() {
            assert(valid && dynamic_cast<node*>(v) != nullptr);
            unlink();
            v = v->r;
            link();
            return *this;
        }
        const_iterator& operator--() {
            assert(valid && dynamic_cast<node*>(v->l) != nullptr);
            unlink();
            v = v->l;
            link();
            return *this;
        }
        const_iterator operator++(int) {
            const_iterator res = *this;
            ++*this;
            return res;
        }
        const_iterator operator--(int) {
            const_iterator res = *this;
            --*this;
            return res;
        }
        T const& operator*() const {
            assert(valid && dynamic_cast<node*>(v) != nullptr);
            return dynamic_cast<node*>(v)->val;
        }
        friend bool operator==(const const_iterator& a, const const_iterator& b) {
            assert(a.valid && b.valid);
            return a.v == b.v;
        }
        friend bool operator!=(const const_iterator& a, const const_iterator& b) {
            assert(a.valid && b.valid);
            return a.v != b.v;
        }

        ~const_iterator() {
            unlink();
        }
        const_iterator() : v(nullptr), valid(false), parent(nullptr), next_const_iterator(nullptr) {}
        const_iterator(const iterator& it) : v(it.v), valid(it.valid), parent(it.parent), next_const_iterator(nullptr) {
            if (valid)
                link();
        }
        const_iterator(const const_iterator& cit) : v(cit.v), valid(cit.valid), parent(cit.parent), next_const_iterator(nullptr) {
            if (valid)
                link();
        }
        const_iterator& operator=(const const_iterator& cit) {
            if (this != &cit) {
                unlink();
                v = cit.v;
                valid = cit.valid;
                parent = cit.parent;
                next_const_iterator = nullptr;
                if (valid)
                    link();
            }
            return *this;
        }
    public:
        friend list;
        friend node_base;

        node_base* v;
        bool valid;
        const list<T>* parent;
        const_iterator* next_const_iterator;
        const_iterator(node_base* v, const list<T>* parent) : v(v), valid(true), parent(parent), next_const_iterator(nullptr) {
            link();
        }

        void unlink(bool with_invalidate = false) {
            if (!v)
                return;
            if (v->my_first_const_iterator == this) {
                v->my_first_const_iterator = v->my_first_const_iterator->next_const_iterator;
            }
            else {
                const_iterator* cit = v->my_first_const_iterator;
                for (; cit->next_const_iterator != this; cit = cit->next_const_iterator) {}
                cit->next_const_iterator = cit->next_const_iterator->next_const_iterator;
            }
            next_const_iterator = nullptr;
            if (with_invalidate)
                invalid();
        }
        void link() {
            if (v->my_first_const_iterator == nullptr)
                v->my_first_const_iterator = this;
            else {
                next_const_iterator = v->my_first_const_iterator;
                v->my_first_const_iterator = this;
            }
        }
        void invalid() {
            v = nullptr;
            valid = false;
            parent = nullptr;
        }
    };

    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
    
    list() {
        end_ = new node_base;
        end_->l = end_;
        end_->r = end_;
    }
    ~list() {
        clear();
        delete end_;
    }
    list(list<T> const& other) : list() {
        for (const_iterator it = other.begin(); it != other.end(); ++it)
            push_back(*it);
    }
    list<T>& operator=(list const& other) {
        list<T> cur(other);
        swap(*this, cur);
        return *this;
    }
    bool empty() const {
        return begin() == end();
    }
    void clear() {
        while(!empty()) {
            erase(begin());
        }
    }

    void push_back(T const& val) {
        insert(end(), val);
    }
    void pop_back() {
        erase(--end());
    }
    T& back() {
        assert(!empty());
        return dynamic_cast<node*>(end_->l)->val;
    }
    T const& back() const {
        assert(!empty());
        return dynamic_cast<node*>(end_->l)->val;
    }

    // front
    void push_front(T const& val) {
        if (empty()) {
            node_base* newNode = new node(val);
            newNode->r = newNode->l = end_;
            end_->l = end_->r = newNode;
        } else {
            node_base* nextNode = end_->r;
            node_base* newNode = new node(val);
            nextNode->l = end_->r = newNode;
            newNode->r = nextNode;
            newNode->l = end_;
        }
    }
    void pop_front() {
        erase(begin());
    }
    T& front() {
        assert(!empty());
        return dynamic_cast<node*>(end_->r)->val;
    }
    T const& front() const {
        assert(!empty());
        return dynamic_cast<node*>(end_->r)->val;
    }

    //iterator
    iterator begin() {
        return iterator(end_->r, this);
    }
    iterator end() {
        return iterator(end_, this);
    }
    const_iterator begin() const {
        return const_iterator(end_->r, this);
    }
    const_iterator end() const {
        return const_iterator(end_, this);
    }
    reverse_iterator rbegin() {
        return reverse_iterator(end());
    }
    reverse_iterator rend() {
        return reverse_iterator(begin());
    }
    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(end());
    }
    const_reverse_iterator rend() const {
        return const_reverse_iterator(begin());
    }

    iterator insert(const_iterator it, T const& val) {
        assert(it.parent == this);
        if (empty()) {
            node_base* newNode = new node(val);
            newNode->r = end_;
            newNode->l = end_;
            end_->r = newNode;
            end_->l = newNode;
            return begin();
        }
        node_base* prev = it.v->l, *next = it.v;
        node_base* newNode = new node(val);
        newNode->r = next;
        newNode->l = prev;
        prev->r = newNode;
        next->l = newNode;
        return iterator(newNode, this);
    }
    iterator erase(const_iterator it) {
        assert(it.parent == this && it != end());
        node_base* prev = it.v->l, *next = it.v;
        if (next->r == end_ && next->l == end_) {
            end_->r = end_;
            end_->l = end_;
            delete next;
        } else {
            next->l->r = next->r;
            next->r->l = next->l;
            delete next;
        }
        return iterator(prev->r, this);
    }
    void splice(const_iterator pos, list<T>& other, const_iterator first, const_iterator last) {
        assert(pos.parent == this);
        for (const_iterator it = first; it != last; ++it) {
            assert(it.parent == &other && it != pos);
        }
        for (const_iterator it = first; it != last; ++it) {
            for (iterator* cur = it.v->my_first_iterator; cur != nullptr; cur = cur->next_iterator)
                cur->parent = this;
            for (const_iterator* cur = it.v->my_first_const_iterator; cur != nullptr; cur = cur->next_const_iterator) {
                cur->parent = this;
            }
        }
        node_base *prev1 = first.v->l;
        node_base *next1 = last.v;
        node_base *prev2 = pos.v->l;
        node_base *next2 = pos.v;

        prev2->r = first.v;
        first.v->l = prev2;
        last.v->l->r = next2;
        next2->l = last.v->l;

        prev1->r = next1;
        next1->l = prev1;
    }

private:
    template<typename U>
    friend void swap(list<U>& a, list<U>& b);

    node_base* end_;
};

template<typename T>
void swap(list<T>& a, list<T>& b) {
    typename list<T>::node_base* cur = a.end_;
    a.end_ = b.end_;
    b.end_ = cur;
}
