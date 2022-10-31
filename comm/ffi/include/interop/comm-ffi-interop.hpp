/**
 * @file comm-ffi-interop.hpp
 *
 * @brief Exposes routines for converting datatypes from C <-> C++
 * @date 2019-02-11
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 *
 */

#ifndef EAP_COMM_FFI_INTEROP_HPP_
#define EAP_COMM_FFI_INTEROP_HPP_

// STL Includes
#include <memory>

// Package includes
#include <comm-ffi-internal-patterns.hpp>
#include <comm-ffi-types.h>
#include <comm-token.hpp>

namespace eap {
namespace comm {
namespace ffi {
using TokenBuilderHandle = eap::comm::TokenBuilder;
using TokenHandle = std::shared_ptr<eap::comm::Token>;
using RmaAllToAllHandle = RmaAllToAllGeneric;

// TokenBuilder
inline comm_token_builder_t *TokenBuilderToFFI(TokenBuilderHandle *builder) {
    return reinterpret_cast<comm_token_builder_t *>(builder);
}

inline comm_token_builder_t const *TokenBuilderToFFI(TokenBuilderHandle const *builder) {
    return reinterpret_cast<comm_token_builder_t const *>(builder);
}

inline TokenBuilderHandle *TokenBuilderFromFFI(comm_token_builder_t *builder) {
    return reinterpret_cast<TokenBuilderHandle *>(builder);
}

inline TokenBuilderHandle const *TokenBuilderFromFFI(comm_token_builder_t const *builder) {
    return reinterpret_cast<TokenBuilderHandle const *>(builder);
}

// Token
inline comm_token_t *TokenToFFI(TokenHandle *token) {
    return reinterpret_cast<comm_token_t *>(token);
}

inline comm_token_t const *TokenToFFI(TokenHandle const *token) {
    return reinterpret_cast<comm_token_t const *>(token);
}

inline TokenHandle *TokenFromFFI(comm_token_t *token) {
    return reinterpret_cast<TokenHandle *>(token);
}

inline TokenHandle const *TokenFromFFI(comm_token_t const *token) {
    return reinterpret_cast<TokenHandle const *>(token);
}

// RmaAllToAll
inline RmaAllToAllHandle *RmaAllToAllFromHandle(comm_rma_all_to_all_t *rma) {
    return reinterpret_cast<RmaAllToAllHandle *>(rma);
}

inline RmaAllToAllHandle const *RmaAllToAllFromHandle(comm_rma_all_to_all_t const *rma) {
    return reinterpret_cast<RmaAllToAllHandle const *>(rma);
}

inline comm_rma_all_to_all_t *RmaAllToAllToHandle(RmaAllToAllHandle *rma) {
    return reinterpret_cast<comm_rma_all_to_all_t *>(rma);
}

inline comm_rma_all_to_all_t const *RmaAllToAllToHandle(RmaAllToAllHandle const *rma) {
    return reinterpret_cast<comm_rma_all_to_all_t const *>(rma);
}
} // namespace ffi
} // namespace comm
} // namespace eap

#endif // EAP_COMM_FFI_INTEROP_HPP_