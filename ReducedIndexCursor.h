// SPDX-FileCopyrightText: 2006-2023, Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2023, Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: BSD-3-Clause
#pragma once

#include "ReducedIndex.h"

namespace fmc {

template <typename Index>
struct LeftReducedIndexCursor;

template <typename Index>
struct ReducedIndexCursor {
    static constexpr size_t Sigma    = Index::Sigma;
    static constexpr bool   Reversed = false;

    Index const* index{};
    size_t lb;
    size_t lbRev;
    size_t len{};
    ReducedIndexCursor() noexcept = default;
    ReducedIndexCursor(Index const& index) noexcept
        : ReducedIndexCursor{index, 0, 0, index.size()}
    {}
    ReducedIndexCursor(Index const& index, size_t lb, size_t lbRev, size_t len) noexcept
        : index{&index}
        , lb{lb}
        , lbRev{lbRev}
        , len{len}
    {}

    bool operator==(ReducedIndexCursor const& _other) const noexcept {
        return lb == _other.lb
               && len == _other.len;
    }
    bool empty() const {
        return len == 0;
    }
    size_t count() const {
        return len;
    }
    auto extendLeft() const -> std::array<ReducedIndexCursor, Sigma> {
        auto const& bwt = index->bwt;
        auto [rs1, prs1] = bwt.all_ranks_and_prefix_ranks(lb);
        auto [rs2, prs2] = bwt.all_ranks_and_prefix_ranks(lb+len);

        for (size_t i{0}; i < rs1.size(); ++i) {
            rs1[i] += index->C[i];
            rs2[i] += index->C[i];
        }

        auto cursors = std::array<ReducedIndexCursor, Sigma>{};
        for (size_t i{0}; i < Sigma; ++i) {
            cursors[i] = ReducedIndexCursor{*index, rs1[i], lbRev + prs2[i] - prs1[i], rs2[i] - rs1[i]};
        }
        return cursors;
    }

    auto extendRight() const -> std::array<ReducedIndexCursor, Sigma> {
        auto const& bwt = index->bwt;
        auto [rs1, prs1] = bwt.all_ranks_and_prefix_ranks(lbRev);
        auto [rs2, prs2] = bwt.all_ranks_and_prefix_ranks(lbRev+len);

        for (size_t i{0}; i < rs1.size(); ++i) {
            rs1[i] += index->C[i];
            rs2[i] += index->C[i];
        }

        auto cursors = std::array<ReducedIndexCursor, Sigma>{};
        for (size_t i{0}; i < Sigma; ++i) {
            cursors[i] = ReducedIndexCursor{*index, lb + prs2[i] - prs1[i], rs1[i], rs2[i] - rs1[i]};
        }
        return cursors;
    }
    void prefetchLeft() const {
    }
    void prefetchRight() const {
    }

    auto extendLeft(size_t symb) const -> ReducedIndexCursor {
        auto& bwt = index->bwt;
        size_t newLb    = bwt.rank(lb, symb);
        size_t newLbRev = lbRev + bwt.prefix_rank(lb+len, symb) - bwt.prefix_rank(lb, symb);
        size_t newLen   = bwt.rank(lb+len, symb) - newLb;
        auto newCursor = ReducedIndexCursor{*index, newLb + index->C[symb], newLbRev, newLen};
        return newCursor;
    }
    auto extendRight(size_t symb) const -> ReducedIndexCursor {
        auto& bwt = index->bwt;
        size_t newLb    = lb + bwt.prefix_rank(lbRev+len, symb) - bwt.prefix_rank(lbRev, symb);
        size_t newLbRev = bwt.rank(lbRev, symb);
        size_t newLen   = bwt.rank(lbRev+len, symb) - newLbRev;
        auto newCursor = ReducedIndexCursor{*index, newLb, newLbRev + index->C[symb], newLen};
        return newCursor;
    }
};

template <typename Index>
auto begin(ReducedIndexCursor<Index> const& _cursor) {
    return IntIterator{_cursor.lb};
}
template <typename Index>
auto end(ReducedIndexCursor<Index> const& _cursor) {
    return IntIterator{_cursor.lb + _cursor.len};
}

template <typename Index>
struct LeftReducedIndexCursor {
    static constexpr size_t Sigma    = Index::Sigma;
    static constexpr bool   Reversed = false;

    Index const* index;
    size_t lb;
    size_t len;
    LeftReducedIndexCursor(ReducedIndexCursor<Index> const& _other)
        : index{_other.index}
        , lb{_other.lb}
        , len{_other.len}
    {}
    LeftReducedIndexCursor()
        : index{nullptr}
    {}
    LeftReducedIndexCursor(Index const& index)
        : LeftReducedIndexCursor{index, 0, index.size()}
    {}
    LeftReducedIndexCursor(Index const& index, size_t lb, size_t len)
        : index{&index}
        , lb{lb}
        , len{len}
    {}
    bool empty() const {
        return len == 0;
    }
    size_t count() const {
        return len;
    }
    auto extendLeft() const -> std::array<LeftReducedIndexCursor, Sigma> {
        auto const& bwt = index->bwt;
        auto [rs1, prs1] = bwt.all_ranks_and_prefix_ranks(lb);
        auto [rs2, prs2] = bwt.all_ranks_and_prefix_ranks(lb+len);

        for (size_t i{0}; i < rs1.size(); ++i) {
            rs1[i] += index->C[i];
            rs2[i] += index->C[i];
        }

        auto cursors = std::array<LeftReducedIndexCursor, Sigma>{};
        cursors[0] = LeftReducedIndexCursor{*index, rs1[0], rs2[0] - rs1[0]};
        for (size_t i{1}; i < Sigma; ++i) {
            cursors[i] = LeftReducedIndexCursor{*index, rs1[i], rs2[i] - rs1[i]};
        }
        return cursors;
    }

    auto extendLeft(size_t symb) const -> LeftReducedIndexCursor {
        auto& bwt = index->bwt;

        size_t newLb    = bwt.rank(lb, symb);
        size_t newLen   = bwt.rank(lb+len, symb) - newLb;
        auto newCursor = LeftReducedIndexCursor{*index, newLb + index->C[symb], newLen};
        return newCursor;
    }
};

template <typename Index>
auto begin(LeftReducedIndexCursor<Index> const& _cursor) {
    return IntIterator{_cursor.lb};
}
template <typename Index>
auto end(LeftReducedIndexCursor<Index> const& _cursor) {
    return IntIterator{_cursor.lb + _cursor.len};
}

template <size_t TSigma, template <size_t> typename String, typename SparseArray>
struct SelectIndexCursor<ReducedIndex<TSigma, String, SparseArray>> {
    using cursor_t = ReducedIndexCursor<ReducedIndex<TSigma, String, SparseArray>>;
};

template <size_t TSigma, template <size_t> typename String, typename SuffixArray>
struct SelectLeftIndexCursor<ReducedIndex<TSigma, String, SuffixArray>> {
    using cursor_t = LeftBiFMIndexCursor<ReducedIndex<TSigma, String, SuffixArray>>;
};

}
