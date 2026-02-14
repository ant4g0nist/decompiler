/* lldbghidra - LGPL - Copyright 2026 */

#ifndef LLDBGHIDRA_EMITANSI_H
#define LLDBGHIDRA_EMITANSI_H

#include <prettyprint.hh>

using namespace ghidra;

namespace lldbghidra {

/// \brief An emitter that wraps tokens with ANSI color escape codes
///
/// Drop-in replacement for EmitNoMarkup that adds syntax highlighting
/// via ANSI terminal escape sequences based on the syntax_highlight value.
class EmitAnsi : public Emit {
  ostream *s;

public:
  static const char *colorFor(syntax_highlight hl);
  EmitAnsi(void) : Emit() { s = (ostream *)0; }

  virtual int4 beginDocument(void) { return 0; }
  virtual void endDocument(int4 id) {}
  virtual int4 beginFunction(const Funcdata *fd) { return 0; }
  virtual void endFunction(int4 id) {}
  virtual int4 beginBlock(const FlowBlock *bl) { return 0; }
  virtual void endBlock(int4 id) {}
  virtual void tagLine(void) {
    *s << endl; for(int4 i=indentlevel;i>0;--i) *s << ' '; }
  virtual void tagLine(int4 indent) {
    *s << endl; for(int4 i=indent;i>0;--i) *s << ' '; }
  virtual int4 beginReturnType(const Varnode *vn) { return 0; }
  virtual void endReturnType(int4 id) {}
  virtual int4 beginVarDecl(const Symbol *sym) { return 0; }
  virtual void endVarDecl(int4 id) {}
  virtual int4 beginStatement(const PcodeOp *op) { return 0; }
  virtual void endStatement(int4 id) {}
  virtual int4 beginFuncProto(void) { return 0; }
  virtual void endFuncProto(int4 id) {}

  virtual void tagVariable(const string &name,syntax_highlight hl,const Varnode *vn,const PcodeOp *op);
  virtual void tagOp(const string &name,syntax_highlight hl,const PcodeOp *op);
  virtual void tagFuncName(const string &name,syntax_highlight hl,const Funcdata *fd,const PcodeOp *op);
  virtual void tagType(const string &name,syntax_highlight hl,const Datatype *ct);
  virtual void tagField(const string &name,syntax_highlight hl,const Datatype *ct,int4 off,const PcodeOp *op);
  virtual void tagComment(const string &name,syntax_highlight hl,const AddrSpace *spc,uintb off);
  virtual void tagLabel(const string &name,syntax_highlight hl,const AddrSpace *spc,uintb off);
  virtual void tagCaseLabel(const string &name,syntax_highlight hl,const PcodeOp *op,uintb value);
  virtual void print(const string &data,syntax_highlight hl=no_color);
  virtual int4 openParen(const string &paren,int4 id=0) {
    *s << paren; parenlevel += 1; return id; }
  virtual void closeParen(const string &paren,int4 id) {
    *s << paren; parenlevel -= 1; }
  virtual void setOutputStream(ostream *t) { s = t; }
  virtual ostream *getOutputStream(void) const { return s; }
  virtual bool emitsMarkup(void) const { return false; }
};

} // namespace lldbghidra

#endif // LLDBGHIDRA_EMITANSI_H
