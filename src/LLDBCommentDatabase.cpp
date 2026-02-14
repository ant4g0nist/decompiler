/* lldbghidra - LGPL - Copyright 2026 */

#include "LLDBCommentDatabase.h"

namespace lldbghidra {

LLDBCommentDatabase::LLDBCommentDatabase() {
}

void LLDBCommentDatabase::clear() {
    cache.clear();
}

void LLDBCommentDatabase::clearType(const Address &fad, uint4 tp) {
    cache.clearType(fad, tp);
}

void LLDBCommentDatabase::addComment(uint4 tp, const Address &fad, const Address &ad, const string &txt) {
    cache.addComment(tp, fad, ad, txt);
}

bool LLDBCommentDatabase::addCommentNoDuplicate(uint4 tp, const Address &fad, const Address &ad, const string &txt) {
    return cache.addCommentNoDuplicate(tp, fad, ad, txt);
}

CommentSet::const_iterator LLDBCommentDatabase::beginComment(const Address &fad) const {
    return cache.beginComment(fad);
}

CommentSet::const_iterator LLDBCommentDatabase::endComment(const Address &fad) const {
    return cache.endComment(fad);
}

} // namespace lldbghidra
