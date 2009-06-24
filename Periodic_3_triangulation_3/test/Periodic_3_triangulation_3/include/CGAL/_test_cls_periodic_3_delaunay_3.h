// Copyright (c) 1998  INRIA Sophia-Antipolis (France).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org); you may redistribute it under
// the terms of the Q Public License version 1.0.
// See the file LICENSE.QPL distributed with CGAL.
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
// Author(s)     : Manuel Caroli
//                 Francois Rebufat

#include <cassert>
#include <iostream>
#include <sstream>
#include <list>
#include <vector>

#include <CGAL/Random.h>
#include <CGAL/point_generators_3.h>

template <class Periodic_3Triangulation_3>
void
_test_cls_periodic_3_delaunay_3(const Periodic_3Triangulation_3 &,
    bool hom = false)
{
  typedef Periodic_3Triangulation_3                    P3T3;

  // We assume the traits class has been tested already
  // actually, any traits is good if it has been tested

  typedef typename P3T3::Geometric_traits              GT;
  typedef typename P3T3::Triangulation_data_structure  TDS;

  typedef typename GT::FT                              FT;

  typedef typename P3T3::Offset               Offset;
  typedef typename P3T3::Covering_sheets      Covering_sheets;
  typedef typename P3T3::Iso_cuboid           Iso_cuboid;

  typedef typename P3T3::Point                Point;
  typedef typename P3T3::Segment              Segment;
  typedef typename P3T3::Triangle             Triangle;
  typedef typename P3T3::Tetrahedron          Tetrahedron;

  typedef typename P3T3::Periodic_point       Periodic_point;
  typedef typename P3T3::Periodic_segment     Periodic_segment;
  typedef typename P3T3::Periodic_triangle    Periodic_triangle;
  typedef typename P3T3::Periodic_tetrahedron Periodic_tetrahedron;

  typedef typename P3T3::Vertex               Vertex;
  typedef typename P3T3::Cell                 Cell;
  typedef typename P3T3::Facet                Facet;
  typedef typename P3T3::Edge                 Edge;

  typedef typename P3T3::size_type            size_type;
  typedef typename P3T3::difference_type      difference_type;

  typedef typename P3T3::Vertex_handle        Vertex_handle;
  typedef typename P3T3::Cell_handle          Cell_handle; 

  typedef typename P3T3::Vertex_iterator      Vertex_iterator;
  typedef typename P3T3::Edge_iterator        Edge_iterator;
  typedef typename P3T3::Facet_iterator       Facet_iterator;
  typedef typename P3T3::Cell_iterator        Cell_iterator;

  typedef typename P3T3::Periodic_point_iterator Periodic_point_iterator;
  typedef typename P3T3::Periodic_segment_iterator Periodic_segment_iterator;

  typedef typename P3T3::Locate_type          Locate_type;
  typedef typename P3T3::Iterator_type        Iterator_type;


  // Create point sets
  typedef CGAL::Creator_uniform_3<double,Point>  Creator;
  CGAL::Random rnd(7);
  CGAL::Random_points_in_cube_3<Point, Creator> in_cube(1, rnd);
  CGAL::Random_points_in_cube_3<Point, Creator> in_cube_tmp(0.5, rnd);

  std::vector<Point> pts_rnd1000;
  for (int i=0 ; i<1000 ; i++)
    pts_rnd1000.push_back(*in_cube++);

  std::vector<Point> pts_rnd10;
  pts_rnd10.push_back(Point(-0.578898, -0.421525,   0.841529 ));
  pts_rnd10.push_back(Point(-0.341375,  0.0654286,  0.458741 ));
  pts_rnd10.push_back(Point( 0.0857005, 0.51015,    0.504106 ));
  pts_rnd10.push_back(Point( 0.633515,  0.271829,   0.369029 ));
  pts_rnd10.push_back(Point( 0.415803, -0.251537,  -0.51599  ));
  pts_rnd10.push_back(Point( 0.367819, -0.892225,   0.396091 ));
  pts_rnd10.push_back(Point( 0.614818,  0.844804,  -0.0642087));
  pts_rnd10.push_back(Point( 0.846336,  0.0261109, -0.754327 ));
  pts_rnd10.push_back(Point( 0.154865,  0.287508,   0.377003 ));
  pts_rnd10.push_back(Point(-0.255508,  0.816668,   0.823991 ));

  // Create a triangulation for testing
  P3T3 PT(pts_rnd10.begin(), pts_rnd10.end(), Iso_cuboid(-1,-1,-1,1,1,1));

  // Creation
  std::cout << "Creation:" << std::endl;
  std::cout << "  Standard constructor" << std::endl;
  
  P3T3 PT_def;
  assert(PT_def.number_of_vertices() == 0);
  assert(PT_def.is_valid());
  
  P3T3 PT_dom(Iso_cuboid(-1,-2,0,3,2,4));
  assert(PT_dom.number_of_vertices() == 0);
  assert(PT_dom.is_valid());
  
  P3T3 PT_gt(Iso_cuboid(0,0,0,1,1,1),GT());
  assert(PT_gt.number_of_vertices() == 0);
  assert(PT_gt.is_valid());
    
  std::cout << "  Copy constructor" << std::endl;

  P3T3 PT_cp(PT);
  assert(PT_cp == PT);
  assert(PT_cp.is_valid());

  std::cout << "  Special constructor" << std::endl;

  P3T3 PT_range(pts_rnd10.begin(), pts_rnd10.end(),
      Iso_cuboid(-1,-1,-1,1,1,1));
  assert(PT_range.number_of_vertices() == 10);
  assert(PT_range.is_valid());
  

  std::cout << "Operations:" << std::endl;
  std::cout << "  Point insertion" << std::endl;
  std::cout << "    Covering 3 -> 3" << std::endl;

  // non-degenerate
  P3T3 PT3(PT);
  assert(PT3.number_of_sheets() == CGAL::make_array(3,3,3));
  PT3.insert(Point(0.5,-0.5,0.5));
  assert(PT3.number_of_sheets() == CGAL::make_array(3,3,3));
  assert(PT3.number_of_vertices() == 11);

  // degenerate
  P3T3 PT3_deg(Iso_cuboid(0,0,0,6,6,6));
  for (int i=0 ; i<6 ; i+=2)
    for (int j=0 ; j<6 ; j+=2)
      for (int k=0 ; k<6 ; k+=2)
	PT3_deg.insert(Point(i,j,k));
  assert(PT3_deg.number_of_sheets() == CGAL::make_array(3,3,3));
  assert(PT3_deg.number_of_vertices() == 27);
  assert(PT3_deg.is_valid());

   std::cout << "    Covering 3 -> 1" << std::endl;

   // non-degenerate
   std::ifstream fin;
   if (hom) fin.open("data/P3DT3_covering_test_HOM.tri");
   else     fin.open("data/P3DT3_covering_test.tri");
   P3T3 PT1(PT);
   fin >> PT1;
   assert(PT1.number_of_sheets() == CGAL::make_array(3,3,3));
   assert(PT1.number_of_vertices() == 70);
   assert(PT1.is_valid());
   Vertex_handle vh_rem13 = PT1.insert(Point(0.711476,-0.0713565,-0.52312));
   assert(PT1.number_of_sheets() == CGAL::make_array(1,1,1));
   assert(PT1.number_of_vertices() == 71);
   assert(PT1.is_valid());

  // degenerate
  P3T3 PT1_deg(PT3_deg);
  for (int i=1 ; i<6 ; i+=2)
    for (int j=1 ; j<6 ; j+=2)
      for (int k=1 ; k<6 ; k+=2)
	PT1_deg.insert(Point(i,j,k));

  assert(PT1_deg.number_of_sheets() == CGAL::make_array(1,1,1));
  assert(PT1_deg.number_of_vertices() == 54);
  assert(PT1_deg.is_valid());

  std::cout << "    Covering 1 -> 1" << std::endl;

  // non-degenerate
  Vertex_handle vh_rem11 = PT1.insert(Point(0.2,-0.7,-0.3));
  assert(PT1.number_of_sheets() == CGAL::make_array(1,1,1));
  assert(PT1.number_of_vertices() == 72);
  assert(PT1.is_valid());

  // degenerate
  Vertex_handle vh_rem11_deg = PT1_deg.insert(Point(1,2,3));
  assert(PT1_deg.number_of_sheets() == CGAL::make_array(1,1,1));
  assert(PT1_deg.number_of_vertices() == 55);
  assert(PT1_deg.is_valid());

  std::cout << "  Located point insertion" << std::endl;

  P3T3 PT_loc(PT);
  Locate_type lt;
  int li, lj;
  Point p(-0.5,0.2,0.2);
  Cell_handle ch = PT_loc.locate(p,lt,li,lj);
  PT_loc.insert(p,lt,ch,li,lj);
  assert(PT_loc.number_of_vertices() == 11);
  assert(PT_loc.is_valid());

  std::cout << "  Iterator range insertion" << std::endl;
  
  P3T3 PT_range_ins(Iso_cuboid(-1,-1,-1,1,1,1));
  pts_rnd1000.push_back(Point(-1,-1,-1));
  PT_range_ins.insert(pts_rnd1000.begin(), pts_rnd1000.end(), true);
  assert(PT_range_ins.number_of_vertices() == 1001);
  assert(PT_range_ins.is_valid());

  std::cout << "  Point moving" << std::endl;

  P3T3 PT_mov(PT);
  Vertex_handle vh = PT_mov.move_point(PT_mov.vertices_begin(),Point(0,0,0));
  assert(vh == PT_mov.vertices_begin());
  assert(PT_mov.number_of_vertices() == 10);
  assert(PT_mov.is_valid());

  Periodic_point_iterator ppit = PT_mov.periodic_points_begin(P3T3::UNIQUE);
  ppit++;
  assert(ppit->first != vh->point());
  PT_mov.move_point(vh,ppit->first);
  assert(PT_mov.number_of_vertices() == 9);
  assert(PT_mov.is_valid());

  std::cout << "  Vertex removal" << std::endl;
  std::cout << "    Covering 1 -> 1" << std::endl;

  // non-degenerate
  PT1.remove(vh_rem11);
  assert(PT1.number_of_sheets() == CGAL::make_array(1,1,1));
  assert(PT1.number_of_vertices() == 71);
  assert(PT1.is_valid());

  // degenerate
  PT1_deg.remove(vh_rem11_deg);
  assert(PT1_deg.number_of_sheets() == CGAL::make_array(1,1,1));
  assert(PT1_deg.number_of_vertices() == 54);
  assert(PT1_deg.is_valid());

  std::cout << "    Covering 1 -> 3" << std::endl;

  // non-degenerate
  PT1.remove(vh_rem13);
  assert(PT1.number_of_sheets() == CGAL::make_array(3,3,3));
  assert(PT1.number_of_vertices() == 70);
  assert(PT1.is_valid());

  // degenerate
  PT1_deg.remove(PT1_deg.vertices_begin());
  assert(PT1_deg.number_of_sheets() == CGAL::make_array(3,3,3));
  assert(PT1_deg.number_of_vertices() == 53);
  assert(PT1_deg.is_valid());

  std::cout << "    Covering 3 -> 3" << std::endl;

  // non-degenerate
  P3T3 PT_rem3(PT);
  PT_rem3.remove(PT_rem3.vertices_begin());
  assert(PT_rem3.number_of_vertices() == 9);
  assert(PT_rem3.number_of_sheets() == CGAL::make_array(3,3,3));
  assert(PT_rem3.is_valid());
  
  // degenerate
  PT1_deg.remove(PT1_deg.vertices_begin());
  assert(PT1_deg.number_of_vertices() == 52);
  assert(PT1_deg.number_of_sheets() == CGAL::make_array(3,3,3));
  assert(PT1_deg.is_valid());

  std::cout << "  Iterator range removal" << std::endl;
  P3T3 PT_remall(PT);
  std::vector<Vertex_handle> vvh;
  for (Vertex_iterator vit = PT_remall.vertices_begin() ;
       vit != PT_remall.vertices_end() ; ++vit) {
    vvh.push_back(vit);
  }

  PT_remall.remove(vvh.begin(),vvh.end());
  assert(PT_remall.number_of_vertices() == 0);
  assert(PT_remall.is_valid());

  std::cout << "Queries" << std::endl;
  std::cout << "Side of sphere" << std::endl;

  ch = PT.locate(Point(-1,-1,1));
  assert(PT.side_of_sphere(ch,Point(-1,-1,1)) == CGAL::ON_BOUNDED_SIDE);
  assert(PT.side_of_sphere(ch,Point(-1,-1,1),Offset(3,3,0))
	  == CGAL::ON_BOUNDED_SIDE);
  assert(PT.side_of_sphere(ch,Point(-1,-1,1),Offset(0,0,2))
	  == CGAL::ON_UNBOUNDED_SIDE);
  Periodic_point pp = PT.periodic_point(ch,0);
  assert(PT.side_of_sphere(ch,pp.first,pp.second) == CGAL::ON_BOUNDARY);

  std::cout << "  Nearest vertex"<< std::endl;
  vh = PT.nearest_vertex(Point(0,0,0));
  assert(Segment(vh->point(),Point(0,0,0)).squared_length() < FT(0.25));
  assert(PT.nearest_vertex(Point( 1, 1, 1))
      == PT.nearest_vertex(Point( 1,-1,-1)));
  vh = PT.nearest_vertex_in_cell(vh->cell(),Point(0,0,0),
      PT.get_offset(vh->cell(),vh->cell()->index(vh)));
  assert(PT.construct_segment(vh->point(),Point(0,0,0),
	  PT.get_offset(vh),Offset(0,0,0)).squared_length() < FT(0.25));
  vh = PT.nearest_vertex_in_cell(vh->cell(),Point(0,0,0),Offset(2,0,0));
  assert(PT.construct_segment(vh->point(),Point(0,0,0),
	  PT.get_offset(vh),Offset(2,0,0)).squared_length() > FT(0.25));

  std::cout << "  Conflict region"<< std::endl;
  std::vector<Facet> bd_facets;
  std::vector<Cell_handle> conflict_cells;
  std::vector<Facet> int_facets;
  PT.find_conflicts(Point(-1,-1,1),ch,std::back_inserter(bd_facets),
      std::back_inserter(conflict_cells),std::back_inserter(int_facets));
  for (unsigned int i=0 ; i<bd_facets.size() ; i++) {
    assert( (PT.side_of_sphere(bd_facets[i].first,Point(-1,-1,1))
	    == CGAL::ON_BOUNDED_SIDE)
	^ (PT.side_of_sphere(bd_facets[i].first->neighbor(bd_facets[i].second),
		Point(-1,-1,1)) == CGAL::ON_BOUNDED_SIDE) );
  }
  for (unsigned int i=0 ; i<conflict_cells.size() ; i++) {
    assert( PT.side_of_sphere(conflict_cells[i],Point(-1,-1,1))
	== CGAL::ON_BOUNDED_SIDE);
  }
  for (unsigned int i=0 ; i<int_facets.size() ; i++) {
    assert((PT.side_of_sphere(int_facets[i].first,Point(-1,-1,1))
	    == CGAL::ON_BOUNDED_SIDE) );
    assert((PT.side_of_sphere(int_facets[i].first->neighbor(
		    int_facets[i].second),Point(-1,-1,1))
	    == CGAL::ON_BOUNDED_SIDE) );
  }

  std::cout << "  Gabriel"<< std::endl;

  Edge_iterator  eit = PT.edges_begin();
  Facet_iterator fit = PT.facets_begin();
  for (int i=0 ; i<10 ; i++,eit++,fit++ ) {
    PT.is_Gabriel(*eit);
    PT.is_Gabriel(eit->first,eit->second,eit->third);
    PT.is_Gabriel(*fit);
    PT.is_Gabriel(fit->first,fit->second);
  }
 
  std::cout << "Voronoi diagram" << std::endl;

  fit = PT.facets_begin();
  Cell_iterator  cit = PT.cells_begin();
  for (int i=0 ; i<10 ; i++,fit++,cit++ ) {
    PT.dual(*fit);
    PT.dual(fit->first,fit->second);
    PT.dual(cit);
  }

  std::stringstream vor3;
  PT.draw_dual(vor3);
  std::stringstream vor1;
  PT1.draw_dual(vor1);

  std::cout << "Additional testing of degenerate cases" << std::endl;
  
  std::vector<Point> pts;
  
  std::cout << "Test 1" << std::endl;
  pts.clear();
  pts.push_back(Point(0, 0, 0));
  pts.push_back(Point(0, 0, 5));
  pts.push_back(Point(5, 5, 5));
  pts.push_back(Point(0, 5, 5));
  P3T3(pts.begin(), pts.end(), Iso_cuboid(0,0,0,10,10,10));
  
  std::cout << "Test 2" << std::endl;
  pts.clear();
  pts.push_back(Point(5, 5, 5));
  pts.push_back(Point(0, 0, 0));
  pts.push_back(Point(0, 0, 5));
  P3T3(pts.begin(), pts.end(), Iso_cuboid(0,0,0,10,10,10));
  
  std::cout << "Test 3" << std::endl;
  pts.clear();
  pts.push_back(Point(0, 0, 9));
  pts.push_back(Point(0, 0, 0));
  pts.push_back(Point(0, 0, 5));
  pts.push_back(Point(0, 0, 3));
  P3T3(pts.begin(), pts.end(), Iso_cuboid(0,0,0,10,10,10));
  
  std::cout << "Test 4" << std::endl;
  pts.clear();
  pts.push_back(Point(5, 5, 5));
  pts.push_back(Point(5, 5, 0));
  pts.push_back(Point(0, 0, 5));
  P3T3(pts.begin(), pts.end(), Iso_cuboid(0,0,0,10,10,10));
  
  std::cout << "Test 5" << std::endl;
  pts.clear();
  pts.push_back(Point(1, 1, 1));
  pts.push_back(Point(1, 1, 1));
  P3T3(pts.begin(), pts.end(), Iso_cuboid(0,0,0,10,10,10));
  
  std::cout << "Test 6" << std::endl;
  pts.clear();
  pts.push_back(Point(2,2,2));
  pts.push_back(Point(2,2,5));
  pts.push_back(Point(2,5,2));
  pts.push_back(Point(5,2,2));
  pts.push_back(Point(5,5,5));
  P3T3(pts.begin(), pts.end(), Iso_cuboid(0,0,0,10,10,10));
  
  std::cout << "Test 7" << std::endl;
  pts.clear();
  pts.push_back(Point(2,2,2));
  pts.push_back(Point(2,2,2));
  pts.push_back(Point(2,5,2));
  pts.push_back(Point(2,5,2));
  P3T3(pts.begin(), pts.end(), Iso_cuboid(0,0,0,10,10,10));
} 
