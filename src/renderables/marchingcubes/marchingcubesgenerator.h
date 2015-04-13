#ifndef MARCHINGCUBESGENERATOR_H
#define MARCHINGCUBESGENERATOR_H

// File Name: marchingcubes.h
// Last Modified: 2014-04-13
// Author: Anders Hafreager (basesd on source code
// provided by Raghavendra Chandrashekara)
// Email: anderhaf@fys.uio.no
//
// Description: This is the interface file for the MarchingCubes class.
// MarchingCubes can be used to construct an isosurface from a scalar
// field.

#include <map>
#include <array>
#include <vector>
#include <QVector>
#include <functional>
#include <QVector3D>
#include <QPair>

using std::function;

struct Vertex {
    QVector3D coordinates;
    float value;
    float x() { return coordinates.x(); }
    float y() { return coordinates.y(); }
    float z() { return coordinates.z(); }
};

struct Edge {
    Vertex *point1;
    Vertex *point2;
};

struct CubeEdges {
    // See http://paulbourke.net/geometry/polygonise/ for figure
    Edge edge0;
    Edge edge1;
    Edge edge2;
    Edge edge3;
    Edge edge4;
    Edge edge5;
    Edge edge6;
    Edge edge7;
    Edge edge8;
    Edge edge9;
    Edge edge10;
    Edge edge11;
};


struct CubeVertices {
    // See http://paulbourke.net/geometry/polygonise/ for figure
    Vertex v_000; // Corresponds to vertex 0
    Vertex v_001; // Corresponds to vertex 4
    Vertex v_011; // Corresponds to vertex 5
    Vertex v_010; // Corresponds to vertex 1
    Vertex v_110; // Corresponds to vertex 2
    Vertex v_111; // Corresponds to vertex 6
    Vertex v_101; // Corresponds to vertex 7
    Vertex v_100; // Corresponds to vertex 3
};

struct Cube {
    CubeVertices vertices;
    CubeEdges edges;
};

struct MarchingCubesVBOData {
    QVector3D vertex;
    QVector3D normal;
    QVector3D color;
};

struct Triangle {
    unsigned int vertexIndices[3];
    Triangle(unsigned int index1, unsigned int index2, unsigned int index3) {
        vertexIndices[0] = index1;
        vertexIndices[1] = index2;
        vertexIndices[2] = index3;
    }

    Triangle() {

    }
};

struct TriangleLines {
    unsigned int vertexIndices[6];
};

typedef std::map<unsigned int, QVector3D> VertexMap;

class MarchingCubesRenderer;

class MarchingCubesGenerator {
protected:
    float m_threshold = 0.0;
    bool m_validSurface = false;
    bool m_hasColorEvaluator = false;
    bool m_hasContinuousField = false;
    unsigned int m_numberOfVoxels[3];
    QVector3D m_color;
    function<float(const QVector3D point)> m_scalarFieldEvaluator;
    function<QVector3D(const QVector3D point)> m_colorEvaluator;
    QVector<MarchingCubesVBOData> m_data;
    std::vector<Triangle> m_trianglesFront;
    std::vector<Triangle> m_trianglesBack;
    std::vector<TriangleLines> m_lines;
    VertexMap m_edgeMap; // Maps getEdgeID's to QVector3D's intersecting the edge
    void updateCube(Cube &cube, const QVector3D &minValues, const QVector3D &vertexIndices, const QVector3D &delta);
    Cube createCube();
    void calculateNormals();
    unsigned int getEdgeId(unsigned int i, unsigned int j, unsigned int k, unsigned int nEdgeNo);
    unsigned int getVertexID(unsigned int i, unsigned int j, unsigned int k);
    QVector3D calculateIntersection(Edge &edge);
    void deleteSurface();

    friend class MarchingCubesRenderer;
    void calculateNormal(MarchingCubesVBOData &point);
public:
    MarchingCubesGenerator();
    ~MarchingCubesGenerator();

    function<float (const QVector3D point)> scalarFieldEvaluator() const;
    void setScalarFieldEvaluator(const function<float (const QVector3D point)> &scalarFieldEvaluator);
    void generateSurface(QVector3D minValues, QVector3D maxValues, QVector3D numberOfVoxels, float threshold);
    float threshold() const;
    void setThreshold(float threshold);
    void setColorEvaluator(const function<QVector3D (const QVector3D point)> &colorEvaluator);
    void setColor(QVector3D color);
    void setColor(const QColor &color);

    // Lookup tables used in the construction of the isosurface.
    static const unsigned int m_edgeTable[256];
    static const int m_triangleTable[256][16];
};

#endif // MARCHINGCUBESGENERATOR_H
