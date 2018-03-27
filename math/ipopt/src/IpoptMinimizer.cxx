#include <Math/IpoptMinimizer.h>
#include <Fit/ParameterSettings.h>
#include <Math/IFunction.h>
#include <TString.h>

#define Debug(x) std::cout<<"line "<<__LINE__<<" Debug("<<#x<<") = "<<x<<std::endl; std::cout.flush();

using namespace ROOT;
using namespace ROOT::Math;
using namespace Ipopt;
using namespace ROOT::Fit;
//_______________________________________________________________________
IpoptMinimizer::IpoptMinimizer() : BasicMinimizer()
{
   fIpotApp = IpoptApplicationFactory();
   fInternalTNLP = new InternalTNLP(this);
   fIpotApp->Options()->SetStringValue("hessian_approximation", "limited-memory");
   fConstraintFunc = nullptr;
   fIpotApp->Options()->SetStringValue("linear_solver", "mumps"); // mumps by default
   fIpotApp->RethrowNonIpoptException(true);
}

//_______________________________________________________________________
IpoptMinimizer::IpoptMinimizer(const char *type) : BasicMinimizer()
{
   fIpotApp = IpoptApplicationFactory();
   fInternalTNLP = new InternalTNLP(this);
   fIpotApp->Options()->SetStringValue("linear_solver", type);
      fIpotApp->Options()->SetStringValue("hessian_approximation", "limited-memory");
   fConstraintFunc = nullptr;
   fIpotApp->RethrowNonIpoptException(true);
}

//_______________________________________________________________________
IpoptMinimizer::~IpoptMinimizer()
{
   fIpotApp = nullptr;
}

//_______________________________________________________________________
IpoptMinimizer::IpoptMinimizer::InternalTNLP::InternalTNLP::InternalTNLP(IpoptMinimizer *minimizer)
{
//    auto n = minimizer->NDim();
// 
//    auto g = minimizer->ConstraintObjFunction();
//    auto m = 0;
//    if (g)
//       m = g->NDimConstraint();

//    fNNZerosJacobian = n*m;
//    fNNZerosHessian = n*n;
   nlp_lower_bound_inf = -1e19;
   nlp_upper_bound_inf = 1e19;
   fMinimizer = minimizer;
}

//_______________________________________________________________________
IpoptMinimizer::IpoptMinimizer::InternalTNLP::~InternalTNLP()
{
}

//_______________________________________________________________________
bool IpoptMinimizer::IpoptMinimizer::InternalTNLP::get_nlp_info(Index &n, Index &m, Index &nnz_jac_g, Index &nnz_h_lag,
                                                                IndexStyleEnum &index_style)
{
   n = fMinimizer->NDim();

   auto g = fMinimizer->ConstraintObjFunction();
   if (g != NULL)
      m = g->NDimConstraint();
   else
      m = 0;
//    nnz_jac_g = fNNZerosJacobian;
//    nnz_h_lag = fNNZerosHessian;
   nnz_jac_g = n*m;
   nnz_h_lag = n*n;
   Debug(nnz_jac_g);
   Debug(nnz_h_lag);
   
   // use the C style indexing (0-based)
   index_style = TNLP::C_STYLE;

   return true;
}

//_______________________________________________________________________
bool IpoptMinimizer::IpoptMinimizer::InternalTNLP::get_bounds_info(Index n, Number *x_l, Number *x_u, Index m,
                                                                   Number *g_l, Number *g_u)
{
   // here, the n and m we gave IPOPT in get_nlp_info are passed back to us.
   // If desired, we could assert to make sure they are what we think they are.
   // TODO: print a meesage telling that information is not good whit the variables and constraints
   R__ASSERT(n == (Index)fMinimizer->NDim());
   auto g = fMinimizer->ConstraintObjFunction();

   if (g != NULL)
      R__ASSERT(m == (Index)g->NDimConstraint());

   for (Index i = 0; i < n; i++) {
      ParameterSettings varsettings;
      if (fMinimizer->GetVariableSettings(i, varsettings)) {
         if (varsettings.HasLowerLimit()) {
            x_l[i] = varsettings.LowerLimit();
         } else {
            x_l[i] = nlp_lower_bound_inf;
         }
         if (varsettings.HasUpperLimit()) {
            x_u[i] = varsettings.UpperLimit();
         } else {
            x_u[i] = nlp_upper_bound_inf;
         }
      } else {
         MATH_ERROR_MSG("IpoptMinimizer::InternalTNLP::get_bounds_info", Form("Variable index = %d not found", i));
      }
   }
//   // the variables have lower bounds of 1
//   for (Index i=0; i<4; i++) {
//     x_l[i] = 1.0;
//   }
// 
//   // the variables have upper bounds of 5
//   for (Index i=0; i<4; i++) {
//     x_u[i] = 5.0;
//   }
   if (g != NULL) {
//       for (Index i = 0; i < m; i++) {
//          ParameterSettings varsettings;
//          if (fMinimizer->GetConstraintSettings(i, varsettings)) {
//             if (varsettings.HasLowerLimit()) {
//                g_l[i] = varsettings.LowerLimit();
//             } else {
//                g_l[i] = nlp_lower_bound_inf;
//             }
//             if (varsettings.HasUpperLimit()) {
//                g_u[i] = varsettings.UpperLimit();
//             } else {
//                g_u[i] = nlp_upper_bound_inf;
//             }
//          } else {
//             MATH_ERROR_MSG("IpoptMinimizer::InternalTNLP::get_bounds_info", Form("Constraint index = %d not found", i));
//          }
//       }
        // the first constraint g1 has a lower bound of 25
  g_l[0] = 25;
  // the first constraint g1 has NO upper bound, here we set it to 2e19.
  // Ipopt interprets any number greater than nlp_upper_bound_inf as
  // infinity. The default value of nlp_upper_bound_inf and nlp_lower_bound_inf
  // is 1e19 and can be changed through ipopt options.
  g_u[0] = 2e19;

  // the second constraint g2 is an equality constraint, so we set the
  // upper and lower bound to the same value
  g_l[1] = g_u[1] = 40.0;

   }
   return true;
}

//_______________________________________________________________________
bool IpoptMinimizer::IpoptMinimizer::InternalTNLP::get_starting_point(Index n, bool init_x, Number *x, bool init_z,
                                                                      Number *z_L, Number *z_U, Index m,
                                                                      bool init_lambda, Number *lambda)
{
   if (!init_x || init_z || init_lambda) {
      return false;
   }

   R__ASSERT(n == (Index)fMinimizer->NDim());
   for (Index i = 0; i < n; i++) {
      ParameterSettings varsettings;
      if (fMinimizer->GetVariableSettings(i, varsettings)) {
         x[i] = varsettings.Value();
      } else {
         MATH_ERROR_MSG("IpoptMinimizer::InternalTNLP::get_starting_point", Form("Variable index = %d not found", i));
      }
   }
   return true;
}

//_______________________________________________________________________
bool IpoptMinimizer::IpoptMinimizer::InternalTNLP::eval_f(Index n, const Number *x, bool new_x, Number &obj_value)
{

   auto fun = fMinimizer->ObjFunction();
   R__ASSERT(n == (Index)fun->NDim());
   /*if(new_x)*/ obj_value = (*fun)(x);
   return true;
}

//_______________________________________________________________________
bool IpoptMinimizer::IpoptMinimizer::InternalTNLP::eval_grad_f(Index n, const Number *x, bool new_x, Number *grad_f)
{
   auto gfun = fMinimizer->GradObjFunction();
   if (!gfun)
      return false;
   else {
      R__ASSERT(n == (Index)gfun->NDim());
      /*if(new_x)*/ gfun->Gradient(x, grad_f);
   }
   return true;
}

//_______________________________________________________________________
bool IpoptMinimizer::IpoptMinimizer::InternalTNLP::eval_g(Index n, const Number *x, bool new_x, Index m, Number *g)
{
    return false;
   auto gfun = fMinimizer->ConstraintObjFunction();
   if (!gfun) {
      return false;
   } else {
      R__ASSERT(n == (Index)gfun->NDim());
      R__ASSERT(m == (Index)gfun->NDimConstraint());
//       if (new_x)
         (*gfun)(x, g); // eval only if it is a new point
   }
   return true;
}

//_______________________________________________________________________
bool IpoptMinimizer::IpoptMinimizer::InternalTNLP::eval_jac_g(Index n, const Number *x, bool new_x, Index m,
                                                              Index nele_jac, Index *iRow, Index *jCol, Number *values)
{
   auto gfun = fMinimizer->ConstraintObjFunction();
   if (!gfun)
      return false;

   if (values == NULL || x == NULL) {
      // creatring the structure of the jacobian matrix for ipopt, at the moment only dense matrix
         Index idx = 0;
         for (Index row = 0; row < n; row++) {
            for (Index col = 0; col < m; col++) {//triplet format https://www.coin-or.org/Ipopt/documentation/node38.html
               iRow[idx] = row;
               jCol[idx] = col;
               std::cout<<"("<<row<<","<<col<<") = "<<idx<<std::endl;
               std::cout.flush();
               idx++;
            }
         }
         
         R__ASSERT(idx == nele_jac);
   } else {
      R__ASSERT(n == (Index)gfun->NDim());
      R__ASSERT(m == (Index)gfun->NDimConstraint());
      if (new_x) { // eval only if it is a new point
         for (auto i = 0; i < n; i++) {
            for (auto j = 0; j < m; j++){
    values[0] = x[1]*x[2]*x[3]; // 0,0
    values[1] = x[0]*x[2]*x[3]; // 0,1
    values[2] = x[0]*x[1]*x[3]; // 0,2
    values[3] = x[0]*x[1]*x[2]; // 0,3

    values[4] = 2*x[0]; // 1,0
    values[5] = 2*x[1]; // 1,1
    values[6] = 2*x[2]; // 1,2
    values[7] = 2*x[3]; // 1,3
            }
//                if (!gfun->DoJacobian(x, values[i * n + j], i, j))
//                   return false; // if not implemented
         }
      Debug("new x")
      }
   }
   Debug(nele_jac)
   return true;
}

//_______________________________________________________________________
bool IpoptMinimizer::IpoptMinimizer::InternalTNLP::eval_h(Index n, const Number *x, bool new_x, Number obj_factor,
                                                          Index m, const Number *lambda, bool new_lambda,
                                                          Index nele_hess, Index *iRow, Index *jCol, Number *values)
{
   Debug(nele_hess)
//     return false;
   if (values == NULL) {
      // return the structure. This is a symmetric matrix, fill the lower left
      // triangle only.

      // the hessian for this problem is actually dense
         Index idx = 0;
         for (Index row = 0; row < n; row++) {
            for (Index col = 0; col < n; col++) {
               iRow[idx] = row;
               jCol[idx] = col;
               idx++;
               std::cout<<"("<<row<<","<<col<<")"<<std::endl;
               std::cout.flush();
            }
         }
         Debug(idx)
         Debug(nele_hess)
         
         R__ASSERT(idx == nele_hess);
   } else {
       for(auto i=0;i<nele_hess;i++) values[i]=0;
      // return the values. This is a symmetric matrix, fill the lower left
      // triangle only

      // fill the objective portion
      values[0] = obj_factor * (2 * x[3]); // 0,0

      values[1] = obj_factor * (x[3]); // 1,0
      values[2] = 0.;                  // 1,1

      values[3] = obj_factor * (x[3]); // 2,0
      values[4] = 0.;                  // 2,1
      values[5] = 0.;                  // 2,2

      values[6] = obj_factor * (2 * x[0] + x[1] + x[2]); // 3,0
      values[7] = obj_factor * (x[0]);                   // 3,1
      values[8] = obj_factor * (x[0]);                   // 3,2
      values[9] = 0.;                                    // 3,3
//       values[10] = 0.;
//       values[11] = 0.;
//       values[12] = 0.;
//       values[13] = 0.;
//       values[14] = 0.;
//       values[15] = 0.;
      
      // add the portion for the first constraint
      values[1] += lambda[0] * (x[2] * x[3]); // 1,0

      values[3] += lambda[0] * (x[1] * x[3]); // 2,0
      values[4] += lambda[0] * (x[0] * x[3]); // 2,1

      values[6] += lambda[0] * (x[1] * x[2]); // 3,0
      values[7] += lambda[0] * (x[0] * x[2]); // 3,1
      values[8] += lambda[0] * (x[0] * x[1]); // 3,2

      // add the portion for the second constraint
      values[0] += lambda[1] * 2; // 0,0

      values[2] += lambda[1] * 2; // 1,1

      values[5] += lambda[1] * 2; // 2,2

      values[9] += lambda[1] * 2; // 3,3
     }

   return true;
   //    return false;
}

//_______________________________________________________________________
void IpoptMinimizer::IpoptMinimizer::InternalTNLP::finalize_solution(SolverReturn /*status*/, Index n, const Number *x,
                                                                     const Number *z_L, const Number *z_U, Index m,
                                                                     const Number *g, const Number * /*lambda*/,
                                                                     Number obj_value, const IpoptData * /*ip_data*/,
                                                                     IpoptCalculatedQuantities * /*ip_cq*/)
{
   // here is where we would store the solution to variables, or write to a file, etc
   // so we could use the solution.

   // For this example, we write the solution to the console
   std::cout << std::endl << std::endl << "Solution of the primal variables, x" << std::endl;
   for (Index i = 0; i < n; i++) {
      std::cout << "x[" << i << "] = " << x[i] << std::endl;
   }

   std::cout << std::endl << std::endl << "Solution of the bound multipliers, z_L and z_U" << std::endl;
   for (Index i = 0; i < n; i++) {
      std::cout << "z_L[" << i << "] = " << z_L[i] << std::endl;
   }
   for (Index i = 0; i < n; i++) {
      std::cout << "z_U[" << i << "] = " << z_U[i] << std::endl;
   }

   std::cout << std::endl << std::endl << "Objective value" << std::endl;
   std::cout << "f(x*) = " << obj_value << std::endl;

   std::cout << std::endl << "Final value of the constraints:" << std::endl;
   for (Index i = 0; i < m; i++) {
      std::cout << "g(" << i << ") = " << g[i] << std::endl;
   }
   fMinimizer->SetFinalValues(x);

   fMinimizer->SetMinValue(obj_value);
}

//_______________________________________________________________________
void IpoptMinimizer::SetFunction(const ROOT::Math::IMultiGenFunction &func)
{
   BasicMinimizer::SetFunction(func);
}

//_______________________________________________________________________
void IpoptMinimizer::SetNNZerosJacobian(UInt_t nzeros)
{
   fInternalTNLP->fNNZerosJacobian = nzeros;
}

//_______________________________________________________________________
void IpoptMinimizer::SetNNZerosHessian(UInt_t nzeros)
{
   fInternalTNLP->fNNZerosHessian = nzeros;
}

//_______________________________________________________________________
void IpoptMinimizer::SetOptionStringValue(const char *var, const char *value)
{
   fIpotApp->Options()->SetStringValue(var, value);
}

//_______________________________________________________________________
void IpoptMinimizer::SetOptionNumericValue(const char *var, const Ipopt::Number value)
{
   fIpotApp->Options()->SetNumericValue(var, value);
}

//_______________________________________________________________________
void IpoptMinimizer::SetOptionIntegerValue(const char *var, const Ipopt::Index value)
{
   fIpotApp->Options()->SetIntegerValue(var, value);
}

//_______________________________________________________________________
bool IpoptMinimizer::Minimize()
{
   ApplicationReturnStatus status;
   status = fIpotApp->Initialize();
   if (status != Solve_Succeeded) {
      std::cout << std::endl << std::endl << "*** Error during initialization!" << std::endl;
      return (int)status;
   }

   fIpotApp->Options()->SetNumericValue("tol", Tolerance());
   fIpotApp->Options()->SetIntegerValue("max_iter", MaxIterations());
   status = fIpotApp->OptimizeTNLP(fInternalTNLP);

   if (status == Solve_Succeeded) {
      // Retrieve some statistics about the solve
      Index iter_count = fIpotApp->Statistics()->IterationCount();
      std::cout << std::endl << std::endl << "*** The problem solved in " << iter_count << " iterations!" << std::endl;

      Number final_obj = fIpotApp->Statistics()->FinalObjective();
      std::cout << std::endl
                << std::endl
                << "*** The final value of the objective function is " << final_obj << '.' << std::endl;
      return true;
   } else {
      return false;
   }
}

//_______________________________________________________________________
void IpoptMinimizer::SetFunction(const ROOT::Math::IMultiConstraintFunction &func)
{
   fConstraintFunc = func.Clone();
   fConstraintFuncDim = func.NDim();
}

//_______________________________________________________________________
bool IpoptMinimizer::SetConstraint(unsigned int ivar, const std::string &name, double val, double step)
{
   if (ivar > fConstraintValues.size())
      return false;
   if (ivar == fConstraintValues.size()) {
      fConstraintValues.push_back(val);
      fConstraintNames.push_back(name);
      fConstraintSteps.push_back(step);
      fConstraintTypes.push_back(kDefault);
   } else {
      fConstraintValues[ivar] = val;
      fConstraintNames[ivar] = name;
      fConstraintSteps[ivar] = step;
      fConstraintTypes[ivar] = kDefault;

      // remove bounds if needed
      std::map<unsigned int, std::pair<double, double>>::iterator iter = fConstraintBounds.find(ivar);
      if (iter != fConstraintBounds.end())
         fConstraintBounds.erase(iter);
   }

   return true;
}

//_______________________________________________________________________
bool IpoptMinimizer::SetConstraintLowerLimit(unsigned int ivar, double lower)
{
   // set variable lower limit
   double upper =
      (fConstraintBounds.count(ivar)) ? fConstraintBounds[ivar].second : std::numeric_limits<double>::infinity();
   return SetConstraintLimits(ivar, lower, upper);
}

//_______________________________________________________________________
bool IpoptMinimizer::SetConstraintUpperLimit(unsigned int ivar, double upper)
{
   // set variable upper limit
   double lower =
      (fConstraintBounds.count(ivar)) ? fConstraintBounds[ivar].first : -std::numeric_limits<double>::infinity();
   return SetConstraintLimits(ivar, lower, upper);
}

//_______________________________________________________________________
bool IpoptMinimizer::SetConstraintLimits(unsigned int ivar, double lower, double upper)
{
   // set variable limits (remove limits if lower >= upper)
   if (ivar > fConstraintTypes.size())
      return false;
   // if limits do not exists add them or update
   fConstraintBounds[ivar] = std::make_pair(lower, upper);
   if (lower > upper ||
       (lower == -std::numeric_limits<double>::infinity() && upper == std::numeric_limits<double>::infinity())) {
      fConstraintBounds.erase(ivar);
      fConstraintTypes[ivar] = kDefault;
   } else if (lower == upper)
      FixConstraint(ivar); //
   else {
      if (lower == -std::numeric_limits<double>::infinity())
         fConstraintTypes[ivar] = kLowBound;
      else if (upper == std::numeric_limits<double>::infinity())
         fConstraintTypes[ivar] = kUpBound;
      else
         fConstraintTypes[ivar] = kBounds;
   }
   return true;
}

//_______________________________________________________________________
bool IpoptMinimizer::FixConstraint(unsigned int ivar)
{
   // fix variable
   if (ivar > fConstraintTypes.size())
      return false;
   fConstraintTypes[ivar] = kFix;
   return true;
}

//_______________________________________________________________________
bool IpoptMinimizer::ReleaseConstraint(unsigned int ivar)
{
   // fix variable
   if (ivar > fConstraintTypes.size())
      return false;
   if (fConstraintBounds.count(ivar) == 0) {
      fConstraintTypes[ivar] = kDefault;
      return true;
   }
   if (fConstraintBounds[ivar].first == -std::numeric_limits<double>::infinity())
      fConstraintTypes[ivar] = kLowBound;
   else if (fConstraintBounds[ivar].second == std::numeric_limits<double>::infinity())
      fConstraintTypes[ivar] = kUpBound;
   else
      fConstraintTypes[ivar] = kBounds;

   return true;
}

//_______________________________________________________________________
bool IpoptMinimizer::IsFixedConstraint(unsigned int ivar) const
{
   if (ivar > fConstraintTypes.size())
      return false;
   return (fConstraintTypes[ivar] == kFix);
}

//_______________________________________________________________________
bool IpoptMinimizer::GetConstraintSettings(unsigned int ivar, ROOT::Fit::ParameterSettings &varObj) const
{
   if (ivar > fConstraintValues.size())
      return false;
   R__ASSERT(fConstraintValues.size() == fConstraintNames.size() &&
             fConstraintValues.size() == fConstraintTypes.size());
   varObj.Set(fConstraintNames[ivar], fConstraintValues[ivar], fConstraintSteps[ivar]);
   std::map<unsigned int, std::pair<double, double>>::const_iterator itr = fConstraintBounds.find(ivar);
   if (itr != fConstraintBounds.end()) {
      double lower = (itr->second).first;
      double upper = (itr->second).second;
      if (fConstraintTypes[ivar] == kLowBound)
         varObj.SetLowerLimit(lower);
      if (fConstraintTypes[ivar] == kUpBound)
         varObj.SetUpperLimit(upper);
      else
         varObj.SetLimits(lower, upper);
   }
   if (fConstraintTypes[ivar] == kFix)
      varObj.Fix();
   return true;
}

//_______________________________________________________________________
std::string IpoptMinimizer::ConstraintName(unsigned int ivar) const
{
   if (ivar > fConstraintNames.size())
      return "";
   return fConstraintNames[ivar];
}
