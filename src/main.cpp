#include <cstddef>
#include <iostream>
#include <limits>
#include <numbers>

#include <symengine/add.h>
#include <symengine/integer.h>
#include <symengine/matrix.h>
#include <symengine/number.h>
#include <symengine/real_double.h>
#include <symengine/symbol.h>
#include <symengine/symengine_casts.h>
#include <symengine/symengine_rcp.h>
#include <symengine/simplify.h>
#include <symengine/eval.h>
#include <symengine/mul.h>
#include <symengine/basic.h>
#include <vector>

auto alpha = SymEngine::symbol("α");
auto a = SymEngine::symbol("a");
auto d = SymEngine::symbol("d");
auto theta = SymEngine::symbol("Θ");

auto R_x_alpha = SymEngine::DenseMatrix(4, 4,
    {
        SymEngine::integer(1),SymEngine::integer(0),SymEngine::integer(0),SymEngine::integer(0),
        SymEngine::integer(0),SymEngine::cos(alpha),SymEngine::neg(SymEngine::sin(alpha)),SymEngine::integer(0), 
        SymEngine::integer(0),SymEngine::sin(alpha),SymEngine::cos(alpha),SymEngine::integer(0), 
        SymEngine::integer(0),SymEngine::integer(0),SymEngine::integer(0),  SymEngine::integer(1),
    }
);

auto D_x_a = SymEngine::DenseMatrix(4, 4,
    {
        SymEngine::integer(1), SymEngine::integer(0),   SymEngine::integer(0),  a,
        SymEngine::integer(0), SymEngine::integer(1),   SymEngine::integer(0),  SymEngine::integer(0),
        SymEngine::integer(0), SymEngine::integer(0),   SymEngine::integer(1),  SymEngine::integer(0),
        SymEngine::integer(0),SymEngine::integer(0),  SymEngine::integer(0),    SymEngine::integer(1),
    }
);

auto R_z_theta = SymEngine::DenseMatrix(4, 4,
    {
        SymEngine::cos(theta), SymEngine::neg(SymEngine::sin(theta)),   SymEngine::integer(0),  SymEngine::integer(0),
        SymEngine::sin(theta), SymEngine::cos(theta),   SymEngine::integer(0),  SymEngine::integer(0),
        SymEngine::integer(0), SymEngine::integer(0),   SymEngine::integer(1),  SymEngine::integer(0),
        SymEngine::integer(0),SymEngine::integer(0),  SymEngine::integer(0),    SymEngine::integer(1),
    }
);

auto D_z_d = SymEngine::DenseMatrix(4, 4,
    {
        SymEngine::integer(1), SymEngine::integer(0),   SymEngine::integer(0),  SymEngine::integer(0),
        SymEngine::integer(0), SymEngine::integer(1),   SymEngine::integer(0),  SymEngine::integer(0),
        SymEngine::integer(0), SymEngine::integer(0),   SymEngine::integer(1),  d,
        SymEngine::integer(0),SymEngine::integer(0),  SymEngine::integer(0),    SymEngine::integer(1),
    }
);

class  DHParameter {
public:
    DHParameter(SymEngine::RCP<const SymEngine::Basic> alpha, 
                SymEngine::RCP<const SymEngine::Basic> a,
                SymEngine::RCP<const SymEngine::Basic> d,
                SymEngine::RCP<const SymEngine::Basic> theta) 
    {
        this->alpha = alpha;
        this->a = a;
        this->d = d;
        this->theta = theta;
    }

public:
    auto get_alpha() -> SymEngine::RCP<const SymEngine::Basic> { return this->alpha; }
    auto get_a() -> SymEngine::RCP<const SymEngine::Basic> { return this->a; }
    auto get_d() -> SymEngine::RCP<const SymEngine::Basic> { return this->d; }
    auto get_theta() -> SymEngine::RCP<const SymEngine::Basic> { return this->theta; }

    auto print() -> void {
        std::cout << "α: " << *this->alpha << std::endl;
        std::cout << "a: " << *this->a << std::endl;
        std::cout << "d: " << *this->d << std::endl;
        std::cout << "Θ: " << *this->theta << std::endl;
        std::cout << std::endl;
    }

private:
    SymEngine::RCP<const SymEngine::Basic> alpha;
    SymEngine::RCP<const SymEngine::Basic> a;
    SymEngine::RCP<const SymEngine::Basic> d;
    SymEngine::RCP<const SymEngine::Basic> theta;
};

auto deg_to_rad(double degree) -> double {
    return degree * std::numbers::pi / 180;
}

auto substitute(SymEngine::DenseMatrix& m, 
                SymEngine::RCP<const SymEngine::Basic> _old, 
                SymEngine::RCP<const SymEngine::Basic> _new)-> void {
   for (auto i = 0; i < m.nrows(); i++) {
        for (auto j = 0; j < m.ncols(); j++) {
            m.set(
                i, 
                j, 
                m.get(i, j)->subs({ { _old, _new } })
            );
            
            // Some element is float number and approaches 0, then change it to integer number with 0.
            if (m.get(i, j)->get_type_code() == SymEngine::TypeID::SYMENGINE_REAL_DOUBLE) {
                auto& num = down_cast<const SymEngine::RealDouble&>(*m.get(i, j));
                if (num.as_double() == static_cast<double>(-1)) // Equals to -1
                    m.set(i, j, SymEngine::integer(-1));
                else if (std::abs(num.as_double()) < std::numeric_limits<double>::epsilon()) // Equals to 0
                    m.set(i, j, SymEngine::integer(0));
                else if (num.as_double() == static_cast<double>(1)) // Equals to 1
                    m.set(i, j, SymEngine::integer(1));
            }
        }
    }
}

auto create_identity_matrix(size_t row, size_t col) -> SymEngine::DenseMatrix {
    if (row == 0 || col == 0)
        return SymEngine::DenseMatrix();

    SymEngine::vec_basic v;
    for (size_t i = 0; i < row; i++) {
        for (size_t j = 0; j < col; j++) {
            if (i == j)
                v.emplace_back(SymEngine::integer(1));
            else
                v.emplace_back(SymEngine::integer(0));
        }
    }

    SymEngine::DenseMatrix I(row, col, v);


    return I;
}

auto simplify(SymEngine::DenseMatrix& m) -> void {
    for (size_t i = 0; i < m.nrows(); i++) {
        for (size_t j = 0; j < m.ncols(); j++) {
            m.set(i, j, SymEngine::simplify(m.get(i, j)));
        }
    }
}

auto expand(SymEngine::DenseMatrix& m) -> void {
    for (size_t i = 0; i < m.nrows(); i++) {
        for (size_t j = 0; j < m.ncols(); j++) {
            m.set(i, j, SymEngine::expand(m.get(i, j)));
        }
    }
}

auto calculate(std::vector<DHParameter> v) -> std::vector<SymEngine::DenseMatrix>  {
    std::vector<SymEngine::DenseMatrix> vec;

    for (auto& param : v) {
        auto R_x_alpha_tmp = R_x_alpha;
        auto D_x_a_tmp = D_x_a;
        auto R_z_theta_tmp = R_z_theta;
        auto D_z_d_tmp = D_z_d;

        substitute(R_x_alpha_tmp, alpha, param.get_alpha());        
        substitute(R_x_alpha_tmp, alpha, param.get_alpha());        
        substitute(D_x_a_tmp, a, param.get_a());
        substitute(R_z_theta_tmp, theta, param.get_theta());
        substitute(D_z_d_tmp, d, param.get_d());


        auto T = create_identity_matrix(4, 4);
        T.mul_matrix(R_x_alpha_tmp, T);
        T.mul_matrix(D_x_a_tmp, T);
        T.mul_matrix(R_z_theta_tmp, T);
        T.mul_matrix(D_z_d_tmp, T);

        vec.emplace_back(T);
    }

    return vec;
}

auto product(std::vector<SymEngine::DenseMatrix> v, size_t begin=0, size_t end=0) -> SymEngine::DenseMatrix {
    auto res = create_identity_matrix(4, 4);

    if (end != 0) {
        for (size_t i = begin; i < end; i++) 
            res.mul_matrix(v.at(i), res);
    } else {
        for (auto& T : v)
            res.mul_matrix(T, res);
    }


    return res;
}


auto main() -> int {
    auto vec = calculate({
        DHParameter(SymEngine::integer(0), SymEngine::integer(0), SymEngine::integer(0), SymEngine::symbol("Θ1")),
        DHParameter(SymEngine::real_double(deg_to_rad(-90)), SymEngine::integer(0), SymEngine::symbol("d2"), SymEngine::symbol("Θ2")),
        DHParameter(SymEngine::real_double(deg_to_rad(90)), SymEngine::integer(0), SymEngine::symbol("d3"), SymEngine::real_double(deg_to_rad(180))),
        DHParameter(SymEngine::integer(0), SymEngine::symbol("a3"), SymEngine::symbol("d4"), SymEngine::symbol("Θ4")),
        DHParameter(SymEngine::real_double(deg_to_rad(90)), SymEngine::integer(0), SymEngine::integer(0), SymEngine::symbol("Θ5")),
        DHParameter(SymEngine::real_double(deg_to_rad(-90)), SymEngine::integer(0), SymEngine::integer(0), SymEngine::symbol("Θ6"))
    });

    auto T = product(vec, 0, 3);
    expand(T);

    std::cout << T << std::endl;

    return 0;
}