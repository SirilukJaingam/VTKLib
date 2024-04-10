// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause
/**
 * @class   vtkStaticCellLinksTemplate
 * @brief   object represents upward pointers from points
 * to list of cells using each point (template implementation)
 *
 *
 * vtkStaticCellLinksTemplate is a supplemental object to vtkCellArray and
 * vtkCellTypes, enabling access to the list of cells using each point.
 * vtkStaticCellLinksTemplate is an array of links, each link represents a
 * list of cell ids using a particular point. The information provided by
 * this object can be used to determine neighbors (e.g., face neighbors,
 * edge neighbors)and construct other local topological information. This
 * class is a faster implementation of vtkCellLinks. However, it cannot be
 * incrementally constructed; it is meant to be constructed once (statically)
 * and must be rebuilt if the cells change.
 *
 * This is a templated implementation for vtkStaticCellLinks. The reason for
 * the templating is to gain performance and reduce memory by using smaller
 * integral types to represent ids. For example, if the maximum id can be
 * represented by an int (as compared to a vtkIdType), it is possible to
 * reduce memory requirements by half and increase performance. This
 * templated class can be used directly; alternatively the
 * non-templated class vtkStaticCellLinks can be used for convenience;
 * although it uses vtkIdType and so will lose some speed and memory
 * advantages.
 *
 * @sa
 * vtkAbstractCellLinks vtkCellLinks vtkStaticCellLinks
 */

#ifndef vtkStaticCellLinksTemplate_h
#define vtkStaticCellLinksTemplate_h

#include "vtkABINamespace.h"

VTK_ABI_NAMESPACE_BEGIN
class vtkDataSet;
class vtkPolyData;
class vtkUnstructuredGrid;
class vtkExplicitStructuredGrid;
class vtkCellArray;
VTK_ABI_NAMESPACE_END

#include "vtkAbstractCellLinks.h"

VTK_ABI_NAMESPACE_BEGIN
template <typename TIds>
class vtkStaticCellLinksTemplate
{
public:
  ///@{
  /**
   * Instantiate and destructor methods.
   */
  vtkStaticCellLinksTemplate();
  ~vtkStaticCellLinksTemplate();
  ///@}

  /**
   * Make sure any previously created links are cleaned up.
   */
  void Initialize();

  /**
   * Build the link list array for a general dataset. Slower than the
   * specialized methods that follow.
   */
  void BuildLinks(vtkDataSet* ds);

  /**
   * Build the link list array for vtkPolyData.
   */
  void BuildLinks(vtkPolyData* pd);

  /**
   * Build the link list array for vtkUnstructuredGrid.
   */
  void BuildLinks(vtkUnstructuredGrid* ugrid);

  /**
   * Build the link list array for vtkExplicitStructuredGrid.
   */
  void BuildLinks(vtkExplicitStructuredGrid* esgrid);

  /**
   * Specialized methods for building links from cell array.
   */
  void SerialBuildLinks(vtkIdType numPts, vtkIdType numCells, vtkCellArray* cellArray);
  void ThreadedBuildLinks(vtkIdType numPts, vtkIdType numCells, vtkCellArray* cellArray);

  ///@{
  /**
   * Get the number of cells using the point specified by ptId.
   */
  TIds GetNumberOfCells(vtkIdType ptId) { return (this->Offsets[ptId + 1] - this->Offsets[ptId]); }
  vtkIdType GetNcells(vtkIdType ptId) { return (this->Offsets[ptId + 1] - this->Offsets[ptId]); }
  ///@}

  /**
   * Indicate whether the point ids provided defines at least one cell, or a
   * portion of a cell.
   */
  template <typename TGivenIds>
  bool MatchesCell(TGivenIds npts, const TGivenIds* pts);

  /**
   * Return a list of cell ids using the point specified by ptId.
   */
  TIds* GetCells(vtkIdType ptId) { return (this->Links + this->Offsets[ptId]); }

  /**
   * Given point ids that define a cell, find the cells that contains all of
   * these point ids. The set of linked cells is returned in cells.
   */
  void GetCells(vtkIdType npts, const vtkIdType* pts, vtkIdList* cells);

  /**
   * Return the total number of links represented after the links have
   * been built.
   */
  TIds GetLinksSize() { return this->LinksSize; }

  /**
   * Obtain the offsets into the internal links array. This is useful for
   * parallel computing.
   */
  TIds GetOffset(vtkIdType ptId) { return this->Offsets[ptId]; }

  ///@{
  /**
   * Support vtkAbstractCellLinks API.
   */
  unsigned long GetActualMemorySize();
  void DeepCopy(vtkAbstractCellLinks* src);
  void SelectCells(vtkIdType minMaxDegree[2], unsigned char* cellSelection);
  ///@}

  ///@{
  /**
   * Control whether to thread or serial process.
   */
  void SetSequentialProcessing(vtkTypeBool seq) { this->SequentialProcessing = seq; }
  vtkTypeBool GetSequentialProcessing() { return this->SequentialProcessing; }
  ///@}

protected:
  // The various templated data members
  TIds LinksSize;
  TIds NumPts;
  TIds NumCells;

  // These point to the core data structures
  TIds* Links;   // contiguous runs of cell ids
  TIds* Offsets; // offsets for each point into the links array

  // Support for execution
  int Type;
  vtkTypeBool SequentialProcessing;

private:
  vtkStaticCellLinksTemplate(const vtkStaticCellLinksTemplate&) = delete;
  void operator=(const vtkStaticCellLinksTemplate&) = delete;
};

VTK_ABI_NAMESPACE_END
#include "vtkStaticCellLinksTemplate.txx"

#endif
// VTK-HeaderTest-Exclude: vtkStaticCellLinksTemplate.h
