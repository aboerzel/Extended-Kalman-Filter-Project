#include "kalman_filter.h"

using Eigen::MatrixXd;
using Eigen::VectorXd;


KalmanFilter::KalmanFilter() {}

KalmanFilter::~KalmanFilter() {}

void KalmanFilter::Init(VectorXd& x_in, MatrixXd& P_in)
{
    x_ = x_in;
    P_ = P_in;

    // create identity matrix
    I_ = MatrixXd::Identity(x_.size(), x_.size());
}

const VectorXd& KalmanFilter::Predict(const MatrixXd& F, const MatrixXd& Q)
{
    // predict the state
    x_ = (F * x_);
    P_ = F * P_ * F.transpose() + Q;
    return x_;
}

void KalmanFilter::Update(const VectorXd& z, const MatrixXd& R, const MatrixXd& H)
{
    // update the state by using Kalman Filter equations
    VectorXd y = z - (H * x_);
    UpdateInnovation(y, R, H);
}

void KalmanFilter::UpdateEKF(const VectorXd& z, const MatrixXd& R)
{
    // update the state by using Extended Kalman Filter equations
    auto px = x_(0);
    auto py = x_(1);
    auto vx = x_(2);
    auto vy = x_(3);

    auto rho = sqrt(px * px + py * py);
    auto theta = atan2(py, px);
    auto rho_dot = (px * vx + py * vy) / rho;

    auto h = VectorXd(3);
    h << rho, theta, rho_dot;

    VectorXd y = z - h;

    while (y(1) > M_PI || y(1) < -M_PI)
    {
        if (y(1) > M_PI)
        {
            y(1) -= M_PI;
        }
        else
        {
            y(1) += M_PI;
        }
    }

    // measurement matrix - radar
    auto H_radar = tools.CalculateJacobian(x_);
    UpdateInnovation(y, R, H_radar);
}

void KalmanFilter::UpdateInnovation(const VectorXd& y, const MatrixXd& R, const MatrixXd& H)
{
    MatrixXd S = H * P_ * H.transpose() + R;
    MatrixXd K = P_ * H.transpose() * S.inverse();

    x_ = x_ + (K * y);
    P_ = (I_ - (K * H)) * P_;
}

Eigen::VectorXd KalmanFilter::GetState()
{
    return x_;
}

Eigen::MatrixXd KalmanFilter::GetP()
{
    return P_;
}
