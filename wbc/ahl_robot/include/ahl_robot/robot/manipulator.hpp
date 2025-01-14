/*********************************************************************
 *
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2015, Daichi Yoshikawa
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of the Daichi Yoshikawa nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 * Author: Daichi Yoshikawa
 *
 *********************************************************************/

#ifndef __AHL_ROBOT_MANIPULATOR_HPP
#define __AHL_ROBOT_MANIPULATOR_HPP

#include <map>
#include <vector>
#include <Eigen/StdVector>
#include <ahl_digital_filter/differentiator.hpp>
#include "ahl_robot/definition.hpp"
#include "ahl_robot/robot/link.hpp"

namespace ahl_robot
{
  typedef std::vector< LinkPtr, Eigen::aligned_allocator<LinkPtr> > VectorLinkPtr;
  typedef std::vector< Eigen::Matrix4d, Eigen::aligned_allocator<Eigen::Matrix4d> > VectorMatrix4d;
  typedef std::vector<Eigen::Matrix3d> VectorMatrix3d;
  typedef std::vector<Eigen::MatrixXd> VectorMatrixXd;
  typedef std::vector<Eigen::Vector3d> VectorVector3d;
  typedef std::map<std::string, Eigen::MatrixXd, std::less<std::string>, Eigen::aligned_allocator<std::pair<const std::string, Eigen::MatrixXd> > > MapMatrixXd;

  class Manipulator
  {
  public:
    Manipulator();
    void init(unsigned int dof, const Eigen::VectorXd& init_q);
    void update(const Eigen::VectorXd& q_msr);
    void update(const Eigen::VectorXd& q_msr, const Eigen::VectorXd& dq_msr);
    void computeJacobian();
    void computeMassMatrix();
    bool reached(const Eigen::VectorXd& qd, double threshold);
    bool hasLink(const std::string& name);
    void addLink(const LinkPtr& link);
    void reverseLink();

    void setName(const std::string& name)
    {
      name_ = name;
    }
    void setDOF(unsigned int dof)
    {
      dof_ = dof;
    }
    void setMacroManipulatorDOF(unsigned int macro_dof)
    {
      macro_dof_ = macro_dof;
    }
    void setDifferentiatorUpdateRate(double update_rate)
    {
      update_rate_ = update_rate;
    }
    void setDifferentiatorCutoffFrequency(double cutoff_frequency)
    {
      cutoff_frequency_ = cutoff_frequency;
    }

    const std::string& getName() const
    {
      return name_;
    }
    unsigned int getLinkNum()
    {
      return link_.size();
    }
    const std::string& getLinkName(unsigned int idx) const
    {
      return link_[idx]->name;
    }
    const LinkPtr& getLink(unsigned int idx) const
    {
      return link_[idx];
    }
    unsigned int getIndex(const std::string& name)
    {
      return name_to_idx_[name];
    }
    unsigned int getDOF()
    {
      return dof_;
    }
    unsigned int getMacroManipulatorDOF()
    {
      return macro_dof_;
    }
    const Eigen::VectorXd& q() const
    {
      return q_;
    }
    const Eigen::VectorXd& dq() const
    {
      return dq_;
    }
    const Eigen::Matrix4d& getTransform(int idx) const
    {
      return T_[idx];
    }
    const Eigen::Matrix4d& getTransformAbs(int idx) const
    {
      return T_abs_[idx];
    }
    const VectorMatrixXd& getJacobian() const
    {
      return J_;
    }
    const Eigen::MatrixXd& getMassMatrix() const
    {
      return M_;
    }
    const Eigen::MatrixXd& getMassMatrixInv() const
    {
      return M_inv_;
    }

    void print();

  private:
    void computeForwardKinematics();
    void computeTabs(); // Should be called after updating xp
    void computeCabs(); // Should be called after updating xp

    void computeJacobian(int idx, Eigen::MatrixXd& J);
    void computeVelocity();

    std::string name_;
    std::map<std::string, int> name_to_idx_;
    unsigned int dof_;
    unsigned int macro_dof_;

    bool updated_joint_;
    double update_rate_;

    VectorLinkPtr link_;

    Eigen::VectorXd q_; // Generalized coordinates
    Eigen::VectorXd dq_; // Velocity of generalized coordinates

    VectorMatrix4d T_; // Relative transformation matrix associated with each link frame
    VectorMatrix4d T_abs_; // Transformation matrix of i-th link w.r.t base

    VectorMatrix4d C_abs_; // Transformation matrix of i-th com w.r.t base
    VectorVector3d Pin_; // End-effector position w.r.t i-th link w.r.t link

    VectorMatrixXd J_; // Basic jacobian associated with link jacobian

    Eigen::MatrixXd M_; // Mass matrix
    Eigen::MatrixXd M_inv_;

    ahl_filter::DifferentiatorPtr differentiator_;
    double cutoff_frequency_;
  };

  typedef boost::shared_ptr<Manipulator> ManipulatorPtr;
}

#endif /* __AHL_ROBOT_MANIPULATOR_HPP */
