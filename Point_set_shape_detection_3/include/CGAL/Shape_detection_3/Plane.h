// Copyright (c) 2015 INRIA Sophia-Antipolis (France).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org).
// You can redistribute it and/or modify it under the terms of the GNU
// General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $URL$
// $Id$
//
//
// Author(s)     : Sven Oesau, Yannick Verdie, Clément Jamin, Pierre Alliez
//

#ifndef CGAL_SHAPE_DETECTION_3_PLANE_H
#define CGAL_SHAPE_DETECTION_3_PLANE_H

#include <CGAL/Shape_detection_3/Shape_base.h>
#include <CGAL/number_utils.h>

/*!
 \file Plane.h
 */
namespace CGAL {
  namespace Shape_detection_3 {
    /*!
     \ingroup PkgPointSetShapeDetection3Shapes
     \brief Plane implements Shape_base. The plane is represented by the normal vector and the distance to the origin.
     \tparam Traits a model of `EfficientRANSACTraits` with the additional 
             requirement for planes (see `EfficientRANSACTraits` documentation).
     */
  template <class Traits>
  class Plane : public Shape_base<Traits> {
  public:
    /// \cond SKIP_IN_MANUAL
    typedef typename Traits::Point_map Point_map;
     ///< property map to access the location of an input point.
    typedef typename Traits::Normal_map Normal_map;
     ///< property map to access the unoriented normal of an input point.
    typedef typename Traits::FT FT; ///< number type.
    typedef typename Traits::Point_3 Point_3; ///< point type.
    typedef typename Traits::Vector_3 Vector_3;
    /// \endcond

    typedef typename Traits::Plane_3 Plane_3;///< plane type for conversion operator.

  public:
    Plane() : Shape_base<Traits>() {}

    /*!
      Conversion operator to `Plane_3` type.
     */
    operator Plane_3() const {
      return Plane_3(this->get_x(m_normal), this->get_y(m_normal), this->get_z(m_normal), m_d);
    }
            
    /*!
      Normal vector of the plane.
     */
    Vector_3 plane_normal() const {
      return m_normal;
    }
            
    /*!
      Signed distance from the origin.
     */
    FT d() const {
      return m_d;
    }
    
    /// \cond SKIP_IN_MANUAL
    /*!
      Computes squared Euclidean distance from query point to the shape.
     */
    FT squared_distance(const Point_3 &p) const {
      FT d = this->scalar_pdct(
        this->constr_vec(p, m_point_on_primitive), m_normal);
      return d * d;
    }

    /*!
      Computes the orthogonal projection of a query point on the shape.
     */
    Point_3 projection (const Point_3 &p) const {
      Vector_3 v (m_point_on_primitive, p);
      return m_point_on_primitive + (v * m_base1) * m_base1 + (v * m_base2) * m_base2;
    }

    /*!
      Replaces the plane by p
    */
    void update (const Plane_3& p) {
      m_base1 = p.base1 (); m_base2 = p.base2 (); m_normal = p.orthogonal_vector ();
      m_d = -(this->get_x(m_point_on_primitive) * this->get_x(m_normal) 
        + this->get_y(m_point_on_primitive) * this->get_y(m_normal) 
        + this->get_z(m_point_on_primitive) * this->get_z(m_normal));

    }

    /*!
      Helper function to write the plane equation and
      number of assigned points into a string.
     */
    std::string info() const {
      std::stringstream sstr;
      sstr << "Type: plane (" << this->get_x(m_normal) << ", " << this->get_y(m_normal) 
        << ", " << this->get_z(m_normal) << ")x - " << m_d << "= 0"
        << " #Pts: " << this->m_indices.size();

      return sstr.str();
    }
    /// \endcond

  protected:
      /// \cond SKIP_IN_MANUAL
    virtual void create_shape(const std::vector<std::size_t> &indices) {
      Point_3 p1 = this->point(indices[0]);
      Point_3 p2 = this->point(indices[1]);
      Point_3 p3 = this->point(indices[2]);

      m_normal = this->cross_pdct(
        this->constr_vec(p2, p1), this->constr_vec(p3, p1));

      FT length = CGAL::sqrt(this->sqlen(m_normal));

      // Are the points almost singular?
      if (length < (FT)0.0001) {
        return;
      }

      m_normal = this->scale(m_normal, (FT)1.0 / length);
      m_d = -(this->get_x(p1) * this->get_x(m_normal) 
        + this->get_y(p1) * this->get_y(m_normal) 
        + this->get_z(p1) * this->get_z(m_normal));

      //check deviation of the 3 normal
      Vector_3 l_v = this->constr_vec();
      for (std::size_t i = 0;i<3;i++) {
        l_v = this->normal(indices[i]);

        if (CGAL::abs(this->scalar_pdct(l_v, m_normal))
            < this->m_normal_threshold * CGAL::sqrt(this->sqlen(l_v))) {
          this->m_is_valid = false;
          return;
        }

        m_point_on_primitive = p1;
        m_base1 = this->cross_pdct(this->constr_vec(p2, p1), m_normal);
        m_base1 = this->scale(m_base1, ((FT)1.0 / CGAL::sqrt(this->sqlen(m_base1))));

        m_base2 = this->cross_pdct(m_base1, m_normal);
        m_base2 = this->scale(m_base2, ((FT)1.0 / CGAL::sqrt(this->sqlen(m_base2))));
      }

      this->m_is_valid = true;
    }

    virtual void parameters(const std::vector<std::size_t> &indices,
                            std::vector<std::pair<FT, FT> > &parameterSpace,
                            FT &,                    
                            FT min[2],
                            FT max[2]) const {
      // Transform first point before to initialize min/max
      Vector_3 p = this->constr_vec(
        m_point_on_primitive, this->point(indices[0]));
      FT u = this->scalar_pdct(p, m_base1);
      FT v = this->scalar_pdct(p, m_base2);
      parameterSpace[0] = std::pair<FT, FT>(u, v);
      min[0] = max[0] = u;
      min[1] = max[1] = v;

      for (std::size_t i = 1;i<indices.size();i++) {
        p = this->constr_vec(m_point_on_primitive, this->point(indices[i]));
        u = this->scalar_pdct(p, m_base1);
        v = this->scalar_pdct(p, m_base2);
        min[0] = (std::min<FT>)(min[0], u);
        max[0] = (std::max<FT>)(max[0], u);
        min[1] = (std::min<FT>)(min[1], v);
        max[1] = (std::max<FT>)(max[1], v);
        parameterSpace[i] = std::pair<FT, FT>(u, v);
      }
    }
    
    virtual void squared_distance(const std::vector<std::size_t> &indices,
                                  std::vector<FT> &dists) const {
      for (std::size_t i = 0;i<indices.size();i++) {
        const FT d = this->scalar_pdct(
          this->constr_vec(m_point_on_primitive, this->point(indices[i])), 
          m_normal);
        dists[i] = d * d;
      }
    }

    virtual void cos_to_normal(const std::vector<std::size_t> &indices, 
                               std::vector<FT> &angles) const {
      for (std::size_t i = 0;i<indices.size();i++) {
        angles[i] = CGAL::abs(
          this->scalar_pdct(this->normal(indices[i]), m_normal));
      }
    }

    FT cos_to_normal(const Point_3 &, const Vector_3 &n) const{
      return CGAL::abs(this->scalar_pdct(n, m_normal));
    } 
    
    virtual std::size_t minimum_sample_size() const {
      return 3;
    }

    virtual bool supports_connected_component() const {
      return true;
    }

  private:
    Point_3 m_point_on_primitive;
    Vector_3 m_base1, m_base2, m_normal;
    FT m_d;
    /// \endcond
  };
}
}
#endif
