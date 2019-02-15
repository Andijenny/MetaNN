#include <data_gen.h>
#include <MetaNN/meta_nn2.h>
#include <calculate_tags.h>
#include <cmath>
#include <iostream>
using namespace std;
using namespace MetaNN;

namespace
{
    void test_softmax_grad_case1()
    {
        cout << "Test softmax grad case 1 ...\t";
        Vector<CheckElement, CheckDevice> input(3);
        input.SetValue(0, 0.5484);
        input.SetValue(1, 0.3292);
        input.SetValue(2, 0.1224);
        
        Vector<CheckElement, CheckDevice> grad(3);
        grad.SetValue(0, 0.5911);
        grad.SetValue(1, 0.6659);
        grad.SetValue(2, 0.7868);
        
        auto op = SoftmaxGrad(grad, input);
        auto res = Evaluate(op);
        
        assert(fabs(res(0, 0) + 0.0266) < 0.001f);
        assert(fabs(res(0, 1) - 0.0086) < 0.001f);
        assert(fabs(res(0, 2) - 0.0180) < 0.001f);
        cout << "done" << endl;
    }
    
    void test_softmax_grad_case2()
    {
        cout << "Test softmax grad case 2 ...\t";
        BatchMatrix<CheckElement, CheckDevice> input(2, 1, 3);
        input.SetValue(0, 0, 0, 0.5484);
        input.SetValue(0, 0, 1, 0.3292);
        input.SetValue(0, 0, 2, 0.1224);
        
        input.SetValue(1, 0, 0, 0.3915);
        input.SetValue(1, 0, 1, 0.0655);
        input.SetValue(1, 0, 2, 0.5430);
        
        BatchMatrix<CheckElement, CheckDevice> grad(2, 1, 3);
        grad.SetValue(0, 0, 0, 0.5911);
        grad.SetValue(0, 0, 1, 0.6659);
        grad.SetValue(0, 0, 2, 0.7868);
        
        grad.SetValue(1, 0, 0, 1.1634);
        grad.SetValue(1, 0, 1, 1.7164);
        grad.SetValue(1, 0, 2, 0.2763);
        
        auto op = SoftmaxGrad(grad, input);
        auto res = Evaluate(op);
        
        assert(fabs(res[0](0, 0) + 0.0266) < 0.001f);
        assert(fabs(res[0](0, 1) - 0.0086) < 0.001f);
        assert(fabs(res[0](0, 2) - 0.0180) < 0.001f);
        
        assert(fabs(res[1](0, 0) - 0.1744) < 0.001f);
        assert(fabs(res[1](0, 1) - 0.0654) < 0.001f);
        assert(fabs(res[1](0, 2) + 0.2398) < 0.001f);
        cout << "done" << endl;
    }
}

namespace Test::Operators
{
    void test_softmax_grad()
    {
        test_softmax_grad_case1();
        test_softmax_grad_case2();
    }
}