#include <utility>
#include <cassert>
#include <type_traits>

#include "util.h"

template <class Allocator>
ALWAYS_INLINE PriceLevel<Allocator>::PriceLevel(Price price, Allocator& alloc)
    : price_(price)
    , alloc_(alloc) {
}

template <class Allocator>
ALWAYS_INLINE PriceLevel<Allocator>::~PriceLevel() {
    while (!empty())
        popFront();
}

template <class Allocator>
ALWAYS_INLINE Price PriceLevel<Allocator>::price() const noexcept {
    return price_;
}

template <class Allocator>
ALWAYS_INLINE bool PriceLevel<Allocator>::empty() const noexcept {
    return head_ == nullptr;
}

template <class Allocator>
ALWAYS_INLINE const Order& PriceLevel<Allocator>::front() const noexcept {
    assert(head_);
    return head_->order;
}

template <class Allocator>
ALWAYS_INLINE Order& PriceLevel<Allocator>::front() noexcept {
    assert(head_);
    return head_->order;
}

template <class Allocator>
ALWAYS_INLINE void PriceLevel<Allocator>::pushBack(const Order& order) {
    auto* node = alloc_.allocate(1);
    static_assert(std::is_same_v<decltype(node), OrderNode*>);

    assert(node);
    ::new (node) OrderNode{order, nullptr};

    if (tail_)
        tail_ = tail_->next = node;
    else
        head_ = tail_ = node;
}

template <class Allocator>
ALWAYS_INLINE void PriceLevel<Allocator>::popFront() {
    assert(head_);
    auto* node = std::exchange(head_, head_->next);
    node->~OrderNode();
    alloc_.deallocate(node, 1);
}

template <class Allocator>
ALWAYS_INLINE auto PriceLevel<Allocator>::begin() const noexcept -> Iterator {
    return Iterator{head_};
}

template <class Allocator>
ALWAYS_INLINE auto PriceLevel<Allocator>::end() const noexcept -> Iterator {
    return Iterator{nullptr};
}

namespace impl {
    ALWAYS_INLINE PriceLevelIterator::PriceLevelIterator(const OrderNode* node)
        : node_(node) {
    }

    ALWAYS_INLINE auto PriceLevelIterator::operator*() const noexcept -> reference {
        assert(node_);
        return node_->order;
    }

    ALWAYS_INLINE auto PriceLevelIterator::operator->() const noexcept -> pointer {
        return &(operator*());
    }

    ALWAYS_INLINE PriceLevelIterator& PriceLevelIterator::operator++() noexcept {
        assert(node_);
        node_ = node_->next;
        return *this;
    }

    ALWAYS_INLINE PriceLevelIterator PriceLevelIterator::operator++(int) noexcept {
        PriceLevelIterator tmp(*this);
        ++(*this);
        return tmp;
    }
}