/*
 * LinearMixedModel.cpp
 *
 *  Created on: Mar 17, 2016
 *      Author: Aditya Gautam (agautam1@andrew.cmu.edu)
 *      EDITED BY: Liuyu Jin (liuyuj@andrew.cmu.edu)
 */
#include "LinearMixedModel.hpp"

using namespace Eigen;

LinearMixedModel::LinearMixedModel() {
    K = MatrixXd::Zero(1,1);
    S = MatrixXd::Zero(1,1);
    initFlag = false;
}

LinearMixedModel::LinearMixedModel(const unordered_map<string, string> &options) {
    K = MatrixXd::Zero(1,1);
    S = MatrixXd::Zero(1,1);
    initFlag = false;
}

// Methods to set the training data
<<<<<<< HEAD
void LinearMixedModel::train(MatrixXd new_X, MatrixXd new_Y){
    
    cout << "LMM: Training set X and Y are provided !" << endl;
    X = new_X; // Input
    Y = new_Y; // Output
    
    beta = MatrixXd::Random(X.cols(),Y.rows());
    K= MatrixXd::Random(X.rows(),Y.rows());
    beta.setZero();
    K.setZero();
}

void LinearMixedModel::train(MatrixXd new_X, MatrixXd new_Y, MatrixXd new_K){
    
    cout << "LMM: Training set X,Y and K are provided !" << endl;
    X = new_X;
    Y = new_Y;
    K = new_K; // Matrix that needs to be decomposed
    
    // Initialize beta and mau to some random values
    beta = MatrixXd::Random(X.cols(),Y.rows());
    mau = MatrixXd(n,1);
=======
void LinearMixedModel::setXY(MatrixXd X, MatrixXd Y) {
//    cout << "LMM: Training set X and Y are provided !" << endl;
    this->X = X; // Input
    this->Y = Y; // Output

    this->beta = MatrixXd::Random(X.cols(), Y.rows());
    this->K = X*X.transpose();
    this->n = get_num_samples();
    this->d = X.cols();
//    beta.setZero();
//    K.setZero();
}

void LinearMixedModel::setXYK(MatrixXd X, MatrixXd Y, MatrixXd K) {
//    cout << "LMM: Training set X,Y and K are provided !" << endl;
    this->X = X;
    this->Y = Y;
    this->K = K; // Matrix that needs to be decomposed
    this->n = get_num_samples();
    this->d = X.cols();

    // Initialize beta and mau to some random values
    this->beta = MatrixXd::Random(X.cols(), Y.rows());
    this->mau = MatrixXd(n, 1);
>>>>>>> master
}

// Other getters and setter methods
// K = U*S*transpose(U)
<<<<<<< HEAD
void LinearMixedModel::set_U(MatrixXd new_U){
    U = new_U;
}

void LinearMixedModel::set_S(MatrixXd new_S){
    S = new_S;
=======
void LinearMixedModel::set_U(MatrixXd U) {
    this->U = U;
}

void LinearMixedModel::set_S(MatrixXd S) {
    this->S = S;
}


void LinearMixedModel::setUS(MatrixXd U, MatrixXd S) {
    this->U = U;
    this->S = S;
>>>>>>> master
}

void LinearMixedModel::set_lambda(double val) {
    lambda_optimized = val;
}

MatrixXd LinearMixedModel::get_beta(){
    return beta;
}

double LinearMixedModel::get_sigma(){
    return sigma;
}

double LinearMixedModel::get_lambda() {
    return lambda_optimized;
}

<<<<<<< HEAD
void  LinearMixedModel::set_num_samples(int num_samples){
    n = num_samples;
}

int LinearMixedModel::get_num_samples() {
    return n;
=======
void  LinearMixedModel::set_num_samples(int num_samples) {
    this->n = num_samples;
}

long LinearMixedModel::get_num_samples() {
    return this->X.rows();
>>>>>>> master
}

// Decomposition of Similarity Matrix ->
// K = U*S*transpose(U)
void LinearMixedModel::decomposition() {
    JacobiSVD<MatrixXd> svd(this->K, ComputeThinU | ComputeThinV);
    MatrixXd tmpS = svd.singularValues();
    U = svd.matrixU();
    S = MatrixXd::Zero(tmpS.rows(), tmpS.rows());
    for (long i = 0; i<tmpS.rows(); i++){
        S(i, i) = tmpS(i, 0);
    }
}

<<<<<<< HEAD
MatrixXd get_betaVar(double lambda){
    MatrixXd Id(n,n); // n*n
    Id.setIdentity(n,n);
    MatrixXd U_trans = U.transpose(); // n*n
    MatrixXd U_trans_X = U_trans*X; // n*d  //XX
    MatrixXd U_trans_Y = U_trans*Y; // n*1
    MatrixXd U_X_trans = (U_trans_X).transpose(); // d*n
    MatrixXd S_lambda_inv = (S + lambda*Id).cwiseInverse(); // n*n
    MatrixXd betaVar = MatrixXd::Random(d,d); // d*d
    betaVar = ((U_X_trans*S_lambda_inv)*U_trans_X).cwiseInverse();
    return betaVar;

}

// This method will give Beta matrix as a function of the Lambda Matrix.
=======
>>>>>>> master

// This method will give Beta matrix as a function of the Lambda Matrix.
void LinearMixedModel::calculate_beta(double lambda) {
    init();
    MatrixXd Id(n, n); // n*n
    Id.setIdentity(n, n);
    MatrixXd U_trans = U.transpose(); // n*n
<<<<<<< HEAD
    MatrixXd U_trans_X = U_trans*X; // n*d  //XX
    MatrixXd U_trans_Y = U_trans*Y; // n*1
    MatrixXd U_X_trans = (U_trans_X).transpose(); // d*n
    MatrixXd S_lambda_inv = (S + lambda*Id).cwiseInverse(); // n*n

    int r=0,c=0;
    /*
    for(r=0;r<S_lambda_inv.rows();r++)
        for(c=0;c<S_lambda_inv.cols();c++)
            std::cout << S_lambda_inv(r,c) << " ";
    */
    MatrixXd betaVar = get_betaVar(lambda);
    MatrixXd second_term = MatrixXd::Random(d,1); // d*1
    /*
    std::cout << " First term " ;
    for(r=0;r<first_term.rows();r++)
        for(c=0;c<first_term.cols();c++)
            std::cout << first_term(r,c) << " ";
    */
    second_term = (U_X_trans*S_lambda_inv)*U_trans_Y;

    beta = betaVar*second_term;
    /*
    std::cout << " Beta matrix : " ;

    for(r=0;r<beta.rows();r++)
        for(c=0;c<beta.cols();c++)
            std::cout << beta(r,c) << " ";
    */
    return ;
}
=======
    MatrixXd U_trans_X = U_trans * X; // n*d
    MatrixXd U_trans_Y = U_trans * Y; // n*1
    MatrixXd U_X_trans = (U_trans_X).transpose(); // d*n
    MatrixXd S_lambda_inv = (S + lambda * Id).inverse(); // n*n
>>>>>>> master

//    MatrixXd first_term = MatrixXd::Random(d, d); // d*d
//    MatrixXd second_term = MatrixXd::Random(d, 1); // d*1

    MatrixXd first_term = ((U_X_trans * S_lambda_inv) * U_trans_X).inverse();
    MatrixXd second_term = (U_X_trans * S_lambda_inv) * U_trans_Y;

    beta = first_term * second_term;

<<<<<<< HEAD
    double ret_val=0.0,temp_val=0.0;
    calculate_beta(lambda);
    MatrixXd U_tran_Y = U.transpose()*Y; // n*1
    MatrixXd U_tran_X = U.transpose()*X; // n*d
    MatrixXd U_tran_X_beta = U_tran_X*beta;
=======
    return;
}

// This method will give the value of sigma as a function of beta and lambda.
void LinearMixedModel::calculate_sigma(double lambda) {
    init();
    double ret_val = 0.0, temp_val = 0.0;
    this->calculate_beta(lambda);
    MatrixXd U_tran_Y = U.transpose() * Y; // n*1
    MatrixXd U_tran_X = U.transpose() * X; // n*d
    MatrixXd U_tran_X_beta = U_tran_X * beta;
>>>>>>> master

    long n = U_tran_X.rows();

    for (int i = 0; i < n; i++) {
        temp_val = U_tran_Y(i, 0) - U_tran_X_beta(i, 0);
        temp_val = temp_val / (double(S(i, i) + lambda));
        temp_val *= temp_val;
        ret_val += temp_val;
    }
<<<<<<< HEAD
    
    ret_val = ret_val/double(n);
    ret_val = sqrt(ret_val);

    sigma = ret_val;
    //std::cout << " Sigma val = " << ret_val << std::endl;
=======
>>>>>>> master

    this->sigma = ret_val / double(n);
    return;
}

/* This method will return the value of log likehood as a function of lambda
   We have to try different lambda values to check at which the log likelihood
   is maximum or error(cost function) is minimum.
*/
<<<<<<< HEAD
double LinearMixedModel::get_log_likelihood_value(double lambda){
    
    double first_term = 0.0,second_term=0.0, third_term=0.0, ret_val =0.0;
    int n = get_num_samples();
    
    first_term = (n)*log(2*M_PI) + n;
    //std::cout << " Get LogLikelihood : lambda =  " << lambda << std::endl;
    
    for(int i=0;i<n;i++){
=======
double LinearMixedModel::get_log_likelihood_value(double lambda) {
    init();
    double first_term = 0.0, second_term = 0.0, third_term = 0.0, ret_val = 0.0;
    long n = this->get_num_samples();

    first_term = (n) * log(2 * M_PI) + n;
    for (int i = 0; i < n; i++) {
>>>>>>> master

        // Check if the term is less then zero or not, if yes skip it as it will be inf.
        if (double(S(i, i) + lambda) <= 0.0) {
//            std::cout << "LLM Err : Log term is neg.... val = " << double(S(i, i) + lambda) << " for n = " <<
//            i << std::endl;
            continue;
        }
        second_term += log(double(S(i, i) + lambda));
    }

    calculate_sigma(lambda);
    if (sigma <= 0.0) {
        third_term = 0.0;
    } else {
        third_term = n * log(this->sigma);
    }

    ret_val = first_term + second_term + third_term;
    ret_val = -1.0 / 2.0 * ret_val;
    return ret_val;

}

/*
 This method is the function to find the optimal lambda value using
 the grid search for low resolution search and then do the brent search
 for high resolution.It is assumed that the Interval of Lambda is already set.
 If not, then set the value before using this function else default will be used.

 This function is negative of log likehood function and will be used as an objective
 function in the brent method. Since brent method aims at miniming the log likehood
 So, we need to have negative log likelihood function.
 Name is kept to be f for simplicity.
 */
<<<<<<< HEAD
double LinearMixedModel::f(double lambda){
    //std::cout << "f func ... lambda = " << lambda << std::endl;
    return -1.0*(get_log_likelihood_value(lambda));
}

=======
double LinearMixedModel::f(double lambda) {
    init();
    return -1.0 * (this->get_log_likelihood_value(lambda));
}

void LinearMixedModel::init() {
    MatrixXd tmp = MatrixXd::Zero(1,1);
    if (!initFlag){
        if (K.rows() == 1){
            K = X*X.transpose();
            decomposition();
        }
        else if (S.rows() == 1){
            decomposition();
        }
        initFlag = true;
    }
}

MatrixXd LinearMixedModel::getBeta() {
    return beta;
}

double LinearMixedModel::getSigma() {
    return sigma;
}
>>>>>>> master
