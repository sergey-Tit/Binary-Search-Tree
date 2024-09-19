#include <functional>
#include <limits>
#include <iostream>

struct Inorder{};
struct Postorder{};
struct Preorder{};

template <typename T, typename Comparator = std::less<T>, typename Alloc = std::allocator<T>>
class BST {
private:
    class BaseNode;

    struct Node : public BaseNode {
        T value;
        Node(const T& value) : value(value) {};
    };

    struct BaseNode {
        Node* parent = nullptr;
        Node* left = nullptr;
        Node* right = nullptr;
    };

    std::allocator_traits<Alloc>::template rebind_alloc<Node> alloc_;
    BaseNode fake_node_;
    BaseNode* fake_node_ptr_;
    BaseNode* post_order_begin_;
    Comparator comp_;
    size_t size_;

    template <typename U = Inorder>
    class BaseIterator {
        friend BST;
    public:
        using pointer_type = const T*;
        using reference_type = const T&;
        using value_type = T;
        using difference_type = size_t;

        BaseIterator(const BaseIterator&) = default;
        BaseIterator& operator=(const BaseIterator&) = default;
        bool operator==(const BaseIterator&) const = default;
        bool operator!=(const BaseIterator&) const = default;

        reference_type operator*() const {
            return (static_cast<Node*>(node))->value;
        }

        pointer_type operator->() const {
            return &((static_cast<Node*>(node))->value);
        }

        BaseIterator& operator++() {
            return Increment(U{});
        }

        BaseIterator operator++(int) {
            BaseIterator copy = *this;
            ++(*this);
            return copy;
        }

        BaseIterator& operator--() {
            return Decrement(U{});
        }

        BaseIterator operator--(int) {
            BaseIterator copy = *this;
            --(*this);
            return copy;
        }

    private:
        BaseNode* node;

        BaseIterator(BaseNode* node) : node(node) {};

        BaseIterator& Increment(Inorder) {
            if (node->right != nullptr) {
                BaseNode* cur_node = node->right;
                while (cur_node->left != nullptr) {
                    cur_node = cur_node->left;
                }
                node = cur_node;
                return *this;
            }
            BaseNode* node_parent = node->parent;
            while (node_parent != node && node == node_parent->right) {
                node = node_parent;
                node_parent = node->parent;
            }
            node = node_parent;
            return *this;
        }

        BaseIterator& Increment(Preorder) {
            if (node->left != nullptr) {
                node = node->left;
                return *this;
            }
            if (node->right != nullptr) {
                node = node->right;
                return *this;
            }
            while (node->parent->left != node || node->parent->right == nullptr) {
                node = node->parent;
            }
            node = node->parent;
            if (node != node->parent) {
                node = node->right;
            }
            return *this;
        }

        BaseIterator& Increment(Postorder) {
            if (node->parent->parent == node->parent) {
                node = node->parent;
                return *this;
            }
            if (node->parent->right == node) {
                node = node->parent;
                return *this;
            }
            if (node->parent->right == nullptr) {
                node = node->parent;
                return *this;
            }
            node = node->parent->right;
            while (node->left != nullptr || node->right != nullptr) {
                if (node->left != nullptr) {
                    node = node->left;
                }
                else {
                    node = node->right;
                }
            }
            return *this;
        }

        BaseIterator& Decrement(Inorder) {
            if (node->left != nullptr) {
                BaseNode* cur_node = node->left;
                while (cur_node->right != nullptr) {
                    cur_node = cur_node->right;
                }
                node = cur_node;
                return *this;
            }
            Node* node_parent = node->parent;
            while (node_parent != node && node == node_parent->left) {
                node = node_parent;
                node_parent = node->parent;
            }
            node = node_parent;
            return *this;
        }

        BaseIterator& Decrement(Preorder) {
            if (node->parent->left == node) {
                node = node->parent;
                return *this;
            }
            if (node->parent->left == nullptr) {
                node = node->parent;
                return *this;
            }
            node = node->parent->left;
            while (node->left != nullptr || node->right != nullptr) {
                if (node->right != nullptr) {
                    node = node->right;
                }
                else {
                    node = node->left;
                }
            }
            return *this;
        }

        BaseIterator& Decrement(Postorder) {
            if (node->parent == node) {
                node = node->left;
                return *this;
            }
            if (node->right != nullptr) {
                node = node->right;
                return *this;
            }
            if (node->left != nullptr) {
                node = node->left;
                return *this;
            }
            while (node->parent->right != node || node->parent->left == nullptr) {
                node = node->parent;
            }
            node = node->parent->left;
            return *this;
        }
    };

public:
    template <typename U = Inorder>
    using iterator = BaseIterator<U>;

    template <typename U = Inorder>
    using const_iterator = BaseIterator<U>;

    template <typename U = Inorder>
    using difference_type = std::iterator_traits<iterator<U>>::difference_type;

    template <typename U = Inorder>
    using reverse_iterator = std::reverse_iterator<iterator<U>>;

    template <typename U = Inorder>
    using const_reverse_iterator = std::reverse_iterator<const_iterator<U>>;

    using value_type = T;
    using reference = T&;
    using const_reference = const T&;
    using size_type = size_t;
    using key_type = T;
    using key_compare = Comparator;
    using value_compare = Comparator;
    using node_type = Node;
    using AllocTraits = std::allocator_traits<typename std::allocator_traits<Alloc>::template rebind_alloc<Node>>;
    using allocator_type = Alloc;

    BST(const BST& other) : fake_node_ptr_(&fake_node_), size_(0), comp_(other.comp_), alloc_(other.alloc_) {
        fake_node_ptr_->parent = static_cast<Node*>(fake_node_ptr_);
        fake_node_ptr_->right = static_cast<Node*>(fake_node_ptr_);
        insert(other.begin<Preorder>(), other.end<Preorder>());
    }

    BST(key_compare comp = Comparator(), allocator_type alloc = Alloc()) : fake_node_ptr_(&fake_node_),
            size_(0), alloc_(alloc), comp_(comp) {
        fake_node_ptr_->parent = static_cast<Node*>(fake_node_ptr_);
        fake_node_ptr_->right = static_cast<Node*>(fake_node_ptr_);
    }

    template <typename U>
    BST(U first, U last, key_compare comp = Comparator(), allocator_type alloc = Alloc()) :
            fake_node_ptr_(&fake_node_), size_(0), alloc_(alloc), comp_(comp) {
        fake_node_ptr_->parent = static_cast<Node*>(fake_node_ptr_);
        fake_node_ptr_->right = static_cast<Node*>(fake_node_ptr_);
        insert(first, last);
    }

    BST(const std::initializer_list<value_type>& init_list, key_compare comp = Comparator(), allocator_type alloc = Alloc()) :
            fake_node_ptr_(&fake_node_), size_(0), alloc_(alloc), comp_(comp) {
        fake_node_ptr_->parent = static_cast<Node*>(fake_node_ptr_);
        fake_node_ptr_->right = static_cast<Node*>(fake_node_ptr_);
        insert(init_list);
    }

    ~BST() {
        clear();
    }

    BST& operator=(const BST& other) {
        if ((*this) == other) {
            return *this;
        }
        fake_node_ = other.fake_node_;
        clear();
        alloc_ = other.alloc_;
        comp_ = other.comp_;
        auto it = other.begin<Preorder>();
        while (it != other.end<Preorder>()) {
            insert(*it);
            ++it;
        }
    }

    BST& operator=(const std::initializer_list<value_type>& init_list) {
        (*this).clear();
        (*this).insert(init_list);
        return *this;
    }

    template <typename U = Inorder>
    const_iterator<U> begin() const {
        return iterator<U>(Begin(U{}));
    }

    template <typename U = Inorder>
    const_iterator<U> end() const {
        return iterator<U>(fake_node_ptr_);
    }

    template <typename U = Inorder>
    const_iterator<U> cbegin() const {
        return const_iterator<U>(Begin(U{}));
    }

    template <typename U = Inorder>
    const_iterator<U> cend() const {
        return const_iterator<U>(fake_node_ptr_);
    }

    template <typename U = Inorder>
    const_reverse_iterator<U> rbegin() const {
        return reverse_iterator<U>(end<U>());
    }

    template <typename U = Inorder>
    const_reverse_iterator<U> rend() const {
        return reverse_iterator<U>(begin<U>());
    }

    template <typename U = Inorder>
    const_reverse_iterator<U> crbegin() const {
        return const_reverse_iterator<U>(rbegin<U>());
    }

    template <typename U = Inorder>
    const_reverse_iterator<U> crend() const {
        return const_reverse_iterator<U>(rend<U>());
    }

    template <typename U = Inorder>
    std::pair<iterator<U>, bool> insert(const_reference value) {
        ++size_;
        Node* node = alloc_.allocate(1);
        AllocTraits::construct(alloc_, node, value);
        Node* cur_node = fake_node_ptr_->left;
        if (fake_node_ptr_->left == nullptr) {
            fake_node_ptr_->left = node;
            node->parent = static_cast<Node*>(fake_node_ptr_);
            fake_node_ptr_->right = node;
            post_order_begin_ = node;
            return std::make_pair(iterator<U>(node), true);
        }
        bool flag = true;
        while (cur_node != nullptr) {
            if (comp_(cur_node->value, node->value)) {
                flag = false;
                if (cur_node->right != nullptr) {
                    cur_node = cur_node->right;
                }
                else {
                    cur_node->right = node;
                    node->parent = cur_node;
                    break;
                }
            }
            else if (comp_(node->value, cur_node->value)) {
                if (cur_node->left != nullptr) {
                    cur_node = cur_node->left;
                }
                else {
                    cur_node->left = node;
                    node->parent = cur_node;
                    break;
                }
            }
            else {
                --size_;
                return std::make_pair(iterator<U>(cur_node), false);
            }
        }
        if (flag) {
            fake_node_ptr_->right = node;
        }
        cur_node = fake_node_ptr_->left;
        while (cur_node->left != nullptr || cur_node->right != nullptr) {
            if (cur_node->left != nullptr) {
                cur_node = cur_node->left;
            }
            else {
                cur_node = cur_node->right;
            }
        }
        post_order_begin_ = cur_node;
        return std::make_pair(iterator<U>(node), true);
    }

    template <typename U = Inorder>
    const_iterator<U> find(const_reference value) const {
        if (fake_node_ptr_->left == nullptr) {
            return end<U>();
        }
        Node* cur_node = fake_node_ptr_->left;
        while (cur_node != nullptr) {
            if (comp_(value, cur_node->value)) {
                cur_node = cur_node->left;
            }
            else if (comp_(cur_node->value, value)) {
                cur_node = cur_node->right;
            }
            else {
                return const_iterator<U>(cur_node);
            }
        }
        return end<U>();
    }

    template <typename U = Inorder>
    const_iterator<U> erase(const_iterator<U> it) {
        --size_;
        Node* node = static_cast<Node*>(it.node);
        Node* parent = static_cast<Node*>(node->parent);
        ++it;
        bool flag = false;
        if (node == fake_node_ptr_->right) {
            flag = true;
        }
        if (node->left == nullptr && node->right == nullptr) {
            if (parent == fake_node_ptr_) {
                fake_node_ptr_->left = nullptr;
            }
            else {
                if (parent->left == node) {
                    parent->left = nullptr;
                }
                else {
                    parent->right = nullptr;
                }
            }
            if (fake_node_ptr_->right == node) {
                fake_node_ptr_->right = static_cast<Node*>(node->parent);
            }
            AllocTraits::destroy(alloc_, node);
            alloc_.deallocate(node, 1);
        }
        else if (node->left == nullptr || node->right == nullptr) {
            if (node->right != nullptr) {
                if (parent == fake_node_ptr_) {
                    fake_node_ptr_->left->right->parent = static_cast<Node*>(fake_node_ptr_);
                    fake_node_ptr_->left = fake_node_ptr_->left->right;
                }
                else {
                    if (parent->left == node) {
                        parent->left = node->right;
                    }
                    else {
                        parent->right = node->right;
                    }
                    node->right->parent = parent;
                }
            }
            else {
                if (parent == fake_node_ptr_) {
                    fake_node_ptr_->left->left->parent = static_cast<Node*>(fake_node_ptr_);
                    fake_node_ptr_->left = fake_node_ptr_->left->left;
                }
                else {
                    if (parent->left == node) {
                        parent->left = node->left;
                    }
                    else {
                        parent->right = node->left;
                    }
                    node->left->parent = parent;
                }
            }
            if (fake_node_ptr_->right == node) {
                fake_node_ptr_->right = static_cast<Node*>(node->parent);
            }
            AllocTraits::destroy(alloc_, node);
            alloc_.deallocate(node, 1);
        }
        else {
            Node* next_node = static_cast<Node *>(it.node);
            node->value = next_node->value;
            if (next_node->parent->left == next_node) {
                next_node->parent->left = next_node->right;
                if (next_node->right != nullptr) {
                    next_node->right->parent = next_node->parent;
                }
            } else {
                next_node->parent->right = next_node->right;
                if (next_node->right != nullptr) {
                    next_node->right->parent = next_node->parent;
                }
            }
            it.node = node;
            AllocTraits::destroy(alloc_, next_node);
            alloc_.deallocate(next_node, 1);
        }
        if (flag) {
            fake_node_ptr_->right = static_cast<Node*>(it.node);
        }
        if (size_ == 0) {
            post_order_begin_ = nullptr;
        }
        else {
            Node *cur_node = fake_node_ptr_->left;
            while (cur_node->left != nullptr || cur_node->right != nullptr) {
                if (cur_node->left != nullptr) {
                    cur_node = cur_node->left;
                } else {
                    cur_node = cur_node->right;
                }
            }
            post_order_begin_ = cur_node;
        }
        return it;
    }

    template <typename U = Inorder>
    const_iterator<U> erase(const_iterator<U> start, const_iterator<U> finish) {
        while (start != finish) {
            start = erase(start);
        }
        return finish;
    }

    template <typename U = Inorder>
    const_iterator<U> lower_bound(const_reference value) {
        if (fake_node_ptr_->left == nullptr) {
            return end<U>();
        }
        Node* cur_node = fake_node_ptr_->left;
        Node* answer = nullptr;
        while (cur_node != nullptr) {
            if (comp_(value, cur_node->value)) {
                if (answer == nullptr) {
                    answer = cur_node;
                }
                else {
                    if (comp_(cur_node->value, answer->value)) {
                        answer = cur_node;
                    }
                }
                cur_node = cur_node->left;
            }
            else if (comp_(cur_node->value, value)) {
                cur_node = cur_node->right;
            }
            else {
                return const_iterator<U>(cur_node);
            }
        }
        if (answer == nullptr) {
            return end<U>();
        }
        return const_iterator<U>(answer);
    }

    template <typename U = Inorder>
    const_iterator<U> upper_bound(const_reference value) {
        if (fake_node_ptr_->left == nullptr) {
            return end<U>();
        }
        Node* cur_node = fake_node_ptr_->left;
        Node* answer = nullptr;
        while (cur_node != nullptr) {
            if (comp_(value, cur_node->value)) {
                if (answer == nullptr) {
                    answer = cur_node;
                }
                else {
                    if (comp_(cur_node->value, answer->value)) {
                        answer = cur_node;
                    }
                }
                cur_node = cur_node->left;
            }
            else {
                cur_node = cur_node->right;
            }
        }
        if (answer == nullptr) {
            return end<U>();
        }
        return const_iterator<U>(answer);
    }

    template <typename U = Inorder>
    std::pair<const_iterator<U>, const_iterator<U>> equal_range(const_reference value) {
        return std::make_pair(lower_bound(value), upper_bound(value));
    }

    template <typename U>
    void insert(U first, U second) {
        while (first != second) {
            insert(*first);
            ++first;
        }
    }

    void insert(const std::initializer_list<value_type>& init_list) {
        insert(init_list.begin(), init_list.end());
    }
    size_type size() const {
        return size_;
    }
    size_type max_size() const {
        return std::numeric_limits<size_type>::max();
    }
    bool empty() const {
        return begin() == end();
    }
    size_type erase(const_reference value) {
        auto it = find(value);
        if (it == end()) {
            return 0;
        }
        erase(it);
        return 1;
    }
    size_type count(const_reference value) {
        auto it = find(value);
        if (it == end()) {
            return 0;
        }
        return 1;
    }
    bool contains(const_reference value) {
        return (find(value) != end());
    }
    void clear() {
        erase(begin(), end());
    }
    key_compare key_comp() {
        return comp_;
    }
    value_compare value_comp() {
        return comp_;
    }

private:
    node_type* Begin(Inorder) const {
        return fake_node_ptr_->right;
    }

    node_type* Begin(Preorder) const {
        return fake_node_ptr_->left;
    }

    node_type* Begin(Postorder) const {
        return static_cast<Node*>(post_order_begin_);
    }

};

template <typename T, typename Compare = std::less<T>, typename Alloc = std::allocator<T>>
bool operator==(const BST<T, Compare, Alloc>& first, const BST<T, Compare, Alloc>& second) {
    if (first.size() != second.size()) {
        return false;
    }
    auto it1 = first.template begin<Preorder>();
    auto it2 = second.template begin<Preorder>();
    for (int i = 0; i < first.size(); i++) {
        if ((*it1) != (*it2)) {
            return false;
        }
        ++it1;
        ++it2;
    }
    return true;
}

template <typename T, typename Compare = std::less<T>, typename Alloc = std::allocator<T>>
bool operator!=(const BST<T, Compare, Alloc>& first, const BST<T, Compare, Alloc>& second) {
    return !(first == second);
}

template <typename T, typename Compare = std::less<T>, typename Alloc = std::allocator<T>>
void swap(BST<T, Compare, Alloc>& first, BST<T, Compare, Alloc>& second) {
    first.swap(second);
}

