#ifndef DRAW_CPP
#define DRAW_CPP




#include <iostream>
#include <fstream>
#include <unistd.h>
#include <pangolin/pangolin.h>
#include <sophus/se3.hpp>

using namespace Sophus;
using namespace std;





struct  Point4txyz{
    double timestamp;
    double x;
    double y;
    double z;
} ;

typedef vector<Point4txyz,Eigen::aligned_allocator<Point4txyz>> Point_txyz_List;
typedef vector<Sophus::SE3d, Eigen::aligned_allocator<Sophus::SE3d>> TrajectoryType;


TrajectoryType ReadTrajectory(const string &path) {
  ifstream fin(path);
  TrajectoryType trajectory;
  if (!fin) {
    cerr << "trajectory " << path << " not found." << endl;
    return trajectory;
  }

  while (!fin.eof()) {
    double time, tx, ty, tz, qx, qy, qz, qw;
    fin >> time >> tx >> ty >> tz >> qx >> qy >> qz >> qw;
    Sophus::SE3d p1(Eigen::Quaterniond(qw, qx, qy, qz), Eigen::Vector3d(tx, ty, tz));
    trajectory.push_back(p1);
  }
  return trajectory;
}



Point_txyz_List Read_Point_txyz(const string &path) {
  ifstream fin(path);
  Point_txyz_List trajectory1;

  if (!fin) {
    cerr << "trajectory " << path << " not found." << endl;
    return trajectory1;
  }

  while (!fin.eof()) {
    double time, tx, ty, tz, qx, qy, qz, qw;
    fin >> time >> tx >> ty >> tz >> qx >> qy >> qz >> qw;
    //point.timestamp=std::stod(timestamp);
    Point4txyz point_;
    point_.timestamp=time;
    point_.x=tx;
    point_.y=ty;
    point_.z=tz;
    trajectory1.push_back(point_);
  }
  //cout<<"trajectory读取完毕 数据总数  " <<trajectory.size()  << endl;

  return trajectory1;
}


void DrawTrajectory(const TrajectoryType &gt, const TrajectoryType &esti) {
  // create pangolin window and plot the trajectory
  pangolin::CreateWindowAndBind("Trajectory Viewer", 1024, 768);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  pangolin::OpenGlRenderState s_cam(
      pangolin::ProjectionMatrix(1024, 768, 500, 500, 512, 389, 0.1, 1000),
      pangolin::ModelViewLookAt(0, -0.1, -1.8, 0, 0, 0, 0.0, -1.0, 0.0)
  );

  pangolin::View &d_cam = pangolin::CreateDisplay()
      .SetBounds(0.0, 1.0, pangolin::Attach::Pix(175), 1.0, -1024.0f / 768.0f)
      .SetHandler(new pangolin::Handler3D(s_cam));


  while (pangolin::ShouldQuit() == false) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    d_cam.Activate(s_cam);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    glLineWidth(2);
    for (size_t i = 0; i < gt.size() - 1; i++) {
      glColor3f(0.0f, 0.0f, 1.0f);  // blue for ground truth
      glBegin(GL_LINES);  //GL_POINTS
      //glBegin(GL_POINTS)
      auto p1 = gt[i], p2 = gt[i + 1];
      glVertex3d(p1.translation()[0], p1.translation()[1], p1.translation()[2]);
      glVertex3d(p2.translation()[0], p2.translation()[1], p2.translation()[2]);
      glEnd();
    }

    for (size_t i = 0; i < esti.size() - 1; i++) {
      glColor3f(1.0f, 0.0f, 0.0f);  // red for estimated
      glBegin(GL_LINES);
      //glBegin(GL_POINTS);
      auto p1 = esti[i], p2 = esti[i + 1];
      glVertex3d(p1.translation()[0], p1.translation()[1], p1.translation()[2]);
      glVertex3d(p2.translation()[0], p2.translation()[1], p2.translation()[2]);
      glEnd();
    }
    pangolin::FinishFrame();
    usleep(5000);   // sleep 5 ms
  }

}


void DrawTrajectory3(const TrajectoryType &groundtruth, const TrajectoryType &estimated,const TrajectoryType &optimized_pose) {
  // create pangolin window and plot the trajectory
  pangolin::CreateWindowAndBind("Trajectory Viewer", 1024, 768);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  pangolin::OpenGlRenderState s_cam(
      pangolin::ProjectionMatrix(1024, 768, 500, 500, 512, 389, 0.1, 1000),
      pangolin::ModelViewLookAt(0, -0.1, -1.8, 0, 0, 0, 0.0, -1.0, 0.0)
  );

  pangolin::View &d_cam = pangolin::CreateDisplay()
      .SetBounds(0.0, 1.0, pangolin::Attach::Pix(175), 1.0, -1024.0f / 768.0f)
      .SetHandler(new pangolin::Handler3D(s_cam));


  while (pangolin::ShouldQuit() == false) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    d_cam.Activate(s_cam);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    glLineWidth(2);
    //cout<<"groundtruth总点数  "  << groundtruth.size()<< endl;
    for (size_t i = 0; i < groundtruth.size() - 1; i++) {
      glColor3f(0.0f, 0.0f, 1.0f);  // blue for ground truth
      glBegin(GL_LINES);
      //glBegin(GL_POINTS);
      auto p1 = groundtruth[i], p2 = groundtruth[i + 1];
      glVertex3d(p1.translation()[0], p1.translation()[1], p1.translation()[2]);
      glVertex3d(p2.translation()[0], p2.translation()[1], p2.translation()[2]);
      glEnd();
    }

    //cout<<"estimated总点数  "  << estimated.size()<< endl;
    for (size_t i = 0; i < estimated.size() - 1; i++) {
      glColor3f(1.0f, 0.0f, 0.0f);  // red for estimated
      glBegin(GL_LINES);
      auto p1 = estimated[i], p2 = estimated[i + 1];
      glVertex3d(p1.translation()[0], p1.translation()[1], p1.translation()[2]);
      glVertex3d(p2.translation()[0], p2.translation()[1], p2.translation()[2]);
      glEnd();
    }

        
    //cout<<"optimized_pose总点数  "  << optimized_pose.size()<< endl;
    for (size_t i = 0; i < optimized_pose.size() - 1; i++) {
      glColor3f(0.0f, 1.0f, 0.0f);  // green for optimized_pose
      glBegin(GL_LINES);
      //glBegin(GL_POINTS);
      int distance_=0;
      auto p1 = optimized_pose[i], p2 = optimized_pose[i + 1];
      glVertex3d(p1.translation()[0], p1.translation()[1], p1.translation()[2]+distance_);
      glVertex3d(p2.translation()[0], p2.translation()[1], p2.translation()[2]+distance_);
      glEnd();
    }

    pangolin::FinishFrame();
    usleep(5000);   // sleep 5 ms
  }

}

double cal_rmse( TrajectoryType &groundtruth,TrajectoryType &estimated){


 // // compute rmse
  double rmse = 0;
  for (size_t i = 0; i < estimated.size(); i++) {
    Sophus::SE3d p1 = estimated[i], p2 = groundtruth[i];
    double error = (p2.inverse() * p1).log().norm();
    rmse += error * error;

  }
  rmse = rmse / double(estimated.size());
  rmse = sqrt(rmse);
  
  //cout << "RMSE = " << rmse << endl;
  return rmse;
}


double cal_rmse_t( TrajectoryType &groundtruth,TrajectoryType &estimated){


 // // compute rmse
  double rmse = 0;
  for (size_t i = 0; i < estimated.size(); i++) {
    Sophus::SE3d p1 = estimated[i], p2 = groundtruth[i];
    Eigen::Vector3d t_1 = p1.translation();
    Eigen::Vector3d t_2 = p2.translation();

    Eigen::Vector3d error  = (t_2- t_1);
    rmse += error.squaredNorm();//用 squaredNorm() 计算误差平方和

  }
  rmse = rmse / double(estimated.size());
  rmse = sqrt(rmse);
  
  //cout << "RMSE = " << rmse << endl;
  return rmse;
}



#endif 