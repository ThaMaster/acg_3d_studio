#include "RenderVisitor.h"

int main()
{
    auto g = new Geometry("geo");
    auto g1 = new Group("gr1");
    auto g2 = new Group("gr2");
    g1->addChild(g2);
    g2->addChild(g);

    auto root = new Group("root");

    root->addChild(g1);

    auto rv = new RenderVisitor();

    rv->visit(*root);

    return 0;
}