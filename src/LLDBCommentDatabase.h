/* lldbghidra - LGPL - Copyright 2026 */

#ifndef LLDBGHIDRA_LLDBCOMMENTDATABASE_H
#define LLDBGHIDRA_LLDBCOMMENTDATABASE_H

#include <comment.hh>

using namespace ghidra;

namespace lldbghidra {

class LLDBCommentDatabase : public CommentDatabase {
    mutable CommentDatabaseInternal cache;

public:
    LLDBCommentDatabase();

    void clear() override;
    void clearType(const Address &fad, uint4 tp) override;
    void addComment(uint4 tp, const Address &fad, const Address &ad, const string &txt) override;
    bool addCommentNoDuplicate(uint4 tp, const Address &fad, const Address &ad, const string &txt) override;
    void deleteComment(Comment *com) override {
        throw LowlevelError("deleteComment unimplemented");
    }
    CommentSet::const_iterator beginComment(const Address &fad) const override;
    CommentSet::const_iterator endComment(const Address &fad) const override;
    void encode(Encoder &encoder) const override { cache.encode(encoder); }
    void decode(Decoder &decoder) override { throw LowlevelError("decode unimplemented"); }
};

} // namespace lldbghidra

#endif // LLDBGHIDRA_LLDBCOMMENTDATABASE_H
