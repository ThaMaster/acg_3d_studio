#include "RenderVisitor.h"

int main()
{
    auto g = new Geometry("Geometry");
    auto t = new Transform("Transform");
    auto t2 = new Transform("Transform2");
    auto g1 = new Group("g1");
    auto root = new Group("root");

    g1->addChild(t);
    t->addChild(g);
    t2->addChild(g);


    root->addChild(g1);
    root->addChild(t2);

    auto rv = new RenderVisitor();

    rv->visit(*root);

    return 0;
}