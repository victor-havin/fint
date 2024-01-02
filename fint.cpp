//==============================================================================
// fint.cpp
//
// Implementation of FINT grammar (a simple formula interpreter).
//
// 1/1/2024
//==============================================================================

#include <iostream>
#include <iomanip>
#include <map>
#include <cmath>
#include "fintLexer.h"
#include "fintParser.h"
#include "fintVisitor.h"

using namespace antlr4;

//#define DEBUG_GRAMMAR // Uncomment to print grammar debugging information

// Type definition for the variable dictionary
typedef std::map<std::string, double> vars_t;

// Visitor class
class visitor : public fintVisitor
{
private:
  vars_t vars;  // Variable dictionary

  // Error reporting
  void reportErrorItem() {
    std::cout << std::endl;
  }

  template <typename T, typename... P> void reportErrorItem(T first, P... rest) {
    std::cout << first;
    reportErrorItem(rest...);
  }
  
  
  template <typename T, typename... P> void reportError(T * ctx, P... param) {
    std::cout << "line " << ctx->getStart()->getLine() <<  ":" 
              << ctx->getStart()->getCharPositionInLine() << " ";
    reportErrorItem(param...);
  }


// Public implementation starts here
public:

  // Return variable dictionary
  vars_t& getVars() {
    return vars;
  }

  // Root visitor
  virtual std::any visitParse(fintParser::ParseContext *ctx) override {
    return visitChildren(ctx);
  }

  // Program visitor
  virtual std::any visitProgram(fintParser::ProgramContext *ctx) override {
    return visitChildren(ctx);
  }

  // Operation visitor
  virtual std::any visitOperation(fintParser::OperationContext *ctx) override {
    std::string name = std::any_cast<std::string>(visit(ctx->variable()));
    double value = std::any_cast<double>(visit(ctx->expression()));
    vars_t::iterator itr = vars.find(name);
    if(itr == vars.end()) {
      vars.insert(std::pair(name, value));
    }
    else {
      itr->second = value;
    }
    return value;
  }

  // Variable visitor
  virtual std::any visitVariable(fintParser::VariableContext *ctx) override {
    std::string name = ctx->getText();
    return name;
  }

  // Expression visitor
  virtual std::any visitExpression(fintParser::ExpressionContext *ctx) override {
    double value = 0;
    value = std::any_cast<double>(visit(ctx->term(0)));
    for(int i = 1; i < ctx->term().size(); i++) {
      double next = std::any_cast<double>(visit(ctx->term(i)));
      std::string op = ctx->children[2 * i -1]->getText();
      if(op == "+")
        value += next;
      else if(op == "-")
        value -= next;
    }
    return value;
  }

  // Term visitor
  virtual std::any visitTerm(fintParser::TermContext *ctx) override {
    double value = 0;
    value = std::any_cast<double>(visit(ctx->factor(0)));
    for(int i = 1; i < ctx->factor().size(); i++) {
      double next = std::any_cast<double>(visit(ctx->factor(i)));
      std::string op = ctx->children[2 * i -1]->getText();
      if(op == "*")
        value *= next;
      else if(op == "/")
        value /= next;
    }
    return value;
  }

  // Factor visitor
  virtual std::any visitFactor(fintParser::FactorContext *ctx) override {
    double value = std::any_cast<double>(visit(ctx->satom(0)));
    for(int i =1; i < ctx->satom().size(); i++) {
      value = std::pow(value, std::any_cast<double>(visit(ctx->satom(i))));
    }
    return value;
  }

  // Satom (signed atom) visitor
  virtual std::any visitSatom(fintParser::SatomContext *ctx) override {
    double value = 0;
    if(ctx->MINUS() && ctx->atom()) {
        value = -std::any_cast<double>(visit(ctx->atom()));
    }
    else if(ctx->PLUS()) {
        value = -std::any_cast<double>(visit(ctx->atom()));
    }
    else if (ctx->atom()) {
      value = std::any_cast<double>(visit(ctx->atom()));
    }
    return value;
  }

  // Atom visitor
  virtual std::any visitAtom(fintParser::AtomContext *ctx) override {
    double value = 0;
    if(ctx->expression()) {
        value = std::any_cast<double>(visit(ctx->expression()));
    }
    else if (ctx->float_()) {
        value = std::stod(ctx->getText());
    }
    else if (ctx->variable()) {
      std::string name = ctx->variable()->getText();
      vars_t::iterator itr = vars.find(name);
      if(itr != vars.end()) {
        value = itr->second;
      }
      else {
        reportError(ctx, "Undefined variable: ", name);
      }
    }
    else if(ctx->function()){
      value = std::any_cast<double>(visit(ctx->function()));
    }
    return value;
  }

  // Function visitor
  virtual std::any visitFunction(fintParser::FunctionContext *ctx) {
    double value = 0;
    std::string fname = ctx->VARIABLE()->getText();
    double arg0 = std::any_cast<double>(visit(ctx->expression(0)));
    if(fname == "sin") {
      value = sin(arg0);
    }
    else if(fname == "asin") {
      value = asin(arg0);
    }
    else if(fname == "cos") {
      value = cos(arg0);
    }
    else if(fname == "acos") {
      value = acos(arg0);
    }
    else if(fname == "tan") {
      value = tan(arg0);
    }
    else if(fname == "atan") {
      value = atan(arg0);
    }
    else if(fname == "ln") {
      value = log(arg0);
    }
    else if(fname == "pow"){
      if(ctx->expression().size() != 2) {
        std::cout << "Invalid function args." << std::endl;
      }
      else {
        double arg1 = std::any_cast<double>(visit(ctx->expression(1)));
        value = pow(arg0, arg1);
      }
    }
    else if(fname == "log") {
      if(ctx->expression().size() != 2) {
        reportError(ctx, "Invalid function args.");
      }
      else {
        double arg1 = std::any_cast<double>(visit(ctx->expression(1)));
        value = log(arg0)/log(arg1);
      }
    }
    else {
      reportError(ctx, "Invalid function ", fname);
    }
    return value;
  }

  // Float visitor
  virtual std::any visitFloat(fintParser::FloatContext *ctx) override {
    return visitChildren(ctx);
  }

};

// Main program
// Use it like this:
// fint -f <file name>>
// fint "<program>"
// where program is the list of operations separated by ';'
int main(int argc, char * argv[])
{
  if(argc < 2) {
    return 1;
  }

  std::fstream fs;
  ANTLRInputStream * input = NULL;

  // Process command line
  for(int arg = 1; arg < argc; arg++) {
    std::string str = argv[arg];
    if(str == "-f" && arg+1 < argc) {
      fs.open(argv[++arg]);
      if(fs.is_open()) {
        input = new ANTLRInputStream(fs);
      }
      else {
        std::cout << "File error:" << argv[arg] << std::endl;
        return 2;
      }
    }
  }
  // No file? Use command.
  if(input == NULL){
    std::string program;
    for(int i =1; i < argc; i++)
    {
      program += argv[i];
    }
    input = new ANTLRInputStream(program);
  }
  // Lexer
  fintLexer lexer(input);
  CommonTokenStream tokens(&lexer);
  tokens.fill();
#ifdef DEBUG_GRAMMAR
  // Print out tokens
  for (auto token : tokens.getTokens()) {
    std::cout << token->toString() << std::endl;
  }
#endif
  // Parser
  fintParser parser(&tokens);
  tree::ParseTree* tree = parser.parse();
#ifdef DEBUG_GRAMMAR
  // Print out the parsed tree
  std::cout << tree->toStringTree(&parser) << std::endl;
  std::string strText = tree->getText();
#endif
  // Visitor
  visitor visitor;
  visitor.visit(tree);

  // Print out variables
  for(const auto &[name, value] : visitor.getVars()) {
    std::cout << std::setprecision(8) << name << "="  << value <<  std::endl;
  }

  // Cleanup
  delete input;
  return 0;
}

//= End of fint.cpp ============================================================