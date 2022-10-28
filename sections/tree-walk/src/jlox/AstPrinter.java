// package jlox;

// import jlox.Expr.Assign;
// import jlox.Expr.Binary;
// import jlox.Expr.Grouping;
// import jlox.Expr.Literal;
// import jlox.Expr.Logical;
// import jlox.Expr.Unary;
// import jlox.Expr.Variable;

// class AstPrinter implements Expr.Visitor<String> {
//     String print(Expr expr) {
//         return expr.accept(this);
//     }

//     @Override
//     public String visit(Binary expr) {
//         return parenthesize(expr.operator.lexeme, expr.left, expr.right);
//     }

//     @Override
//     public String visit(Grouping expr) {
//         return parenthesize("group", expr.expression);
//     }

//     @Override
//     public String visit(Literal expr) {
//         if (expr.value == null)
//             return "nil";
//         return expr.value.toString();
//     }

//     @Override
//     public String visit(Unary expr) {
//         return parenthesize(expr.operator.lexeme, expr.right);
//     }

//     private String parenthesize(String name, Expr... exprs) {
//         StringBuilder builder = new StringBuilder();

//         builder.append("(").append(name);

//         for (Expr expr : exprs) {
//             builder.append(" ");
//             builder.append(expr.accept(this));
//         }
//         builder.append(")");

//         return builder.toString();
//     }

//     @Override
//     public String visit(Variable expr) {
//         // TODO Auto-generated method stub
//         return null;
//     }

//     @Override
//     public String visit(Assign expr) {
//         // TODO Auto-generated method stub
//         return null;
//     }

//     @Override
//     public String visit(Logical expr) {
//         // TODO Auto-generated method stub
//         return null;
//     }
// }