/* lldbghidra - MIT License - Copyright 2026 */

#include "EmitAnsi.h"

namespace lldbghidra {

const char *EmitAnsi::colorFor(syntax_highlight hl) {
  switch (hl) {
    case keyword_color: return "\033[1;35m";  // Bold magenta
    case type_color:    return "\033[32m";     // Green
    case funcname_color:return "\033[1;34m";   // Bold blue
    case var_color:     return nullptr;        // Default (no color)
    case const_color:   return "\033[36m";     // Cyan
    case param_color:   return "\033[3;36m";   // Italic cyan
    case global_color:  return "\033[1;33m";   // Bold yellow
    case comment_color: return "\033[2;37m";   // Dim gray
    case error_color:   return "\033[31m";     // Red
    case special_color: return "\033[1m";      // Bold
    case no_color:      return nullptr;        // No wrapping
    default:            return nullptr;
  }
}

static inline void emitColored(ostream *s, const string &name, Emit::syntax_highlight hl) {
  const char *color = EmitAnsi::colorFor(hl);
  if (color)
    *s << color << name << "\033[0m";
  else
    *s << name;
}

void EmitAnsi::tagVariable(const string &name, syntax_highlight hl, const Varnode *vn, const PcodeOp *op) {
  emitColored(s, name, hl);
}

void EmitAnsi::tagOp(const string &name, syntax_highlight hl, const PcodeOp *op) {
  emitColored(s, name, hl);
}

void EmitAnsi::tagFuncName(const string &name, syntax_highlight hl, const Funcdata *fd, const PcodeOp *op) {
  emitColored(s, name, hl);
}

void EmitAnsi::tagType(const string &name, syntax_highlight hl, const Datatype *ct) {
  emitColored(s, name, hl);
}

void EmitAnsi::tagField(const string &name, syntax_highlight hl, const Datatype *ct, int4 off, const PcodeOp *op) {
  emitColored(s, name, hl);
}

void EmitAnsi::tagComment(const string &name, syntax_highlight hl, const AddrSpace *spc, uintb off) {
  emitColored(s, name, hl);
}

void EmitAnsi::tagLabel(const string &name, syntax_highlight hl, const AddrSpace *spc, uintb off) {
  emitColored(s, name, hl);
}

void EmitAnsi::tagCaseLabel(const string &name, syntax_highlight hl, const PcodeOp *op, uintb value) {
  emitColored(s, name, hl);
}

void EmitAnsi::print(const string &data, syntax_highlight hl) {
  emitColored(s, data, hl);
}

} // namespace lldbghidra
