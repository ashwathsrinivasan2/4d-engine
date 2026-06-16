#include "GeometryGenerator.h"
#include <iostream>

GeometryGenerator::GeometryGenerator() {
	Polytope a;
	a.vertices = { glm::vec4(0, 0, 0, 0), glm::vec4(0, 1, 0, 0), glm::vec4(1, 0, 0, 0), glm::vec4(1, 1, 0, 0) };
	a.edges = { {0, 1}, {0, 2}, {1, 3}, {2, 3} };
	a.faces = { {0, 1, 2, 3} };

	printPolytope(a);

	Polytope b;
	b.vertices = { glm::vec4(0, 0, 0, 0), glm::vec4(0, 1, 0, 0), glm::vec4(1, 0, 0, 0), glm::vec4(1, 1, 0, 0) };
	b.edges = { {0, 1}, {0, 2}, {1, 3}, {2, 3} };
	b.faces = { {0, 1, 2, 3} };

	printPolytope(b);

	printPolytope(product(a, b));
}

GeometryGenerator::Polytope GeometryGenerator::product(Polytope a, Polytope b) {
	Polytope prod;

	//NEW VERTICES

	//a.vertices x b.vertices
	for (int i = 0; i < a.vertices.size(); i++) {
		glm::vec4 vA = a.vertices[i];
		for (int j = 0; j < b.vertices.size(); j++) {
			glm::vec4 vB = b.vertices[j];
			glm::vec4 newVertex(vA.x, vA.y, vB.x, vB.y);
			prod.vertices.push_back(newVertex);
		}
	}

	//NEW EDGES

	//a.vertices x b.edges
	for (int i = 0; i < a.vertices.size(); i++) {
		for (int j = 0; j < b.edges.size(); j++) {
			std::vector<int> newEdge;
			int endpointA = i * b.vertices.size() + b.edges[j][0];
			int endpointB = i * b.vertices.size() + b.edges[j][1];
			prod.edges.push_back({ endpointA, endpointB });
		}
	}

	//a.edges x b.vertices
	for (int i = 0; i < b.vertices.size(); i++) {
		for (int j = 0; j < a.edges.size(); j++) {
			int endpointA = a.edges[j][0] * b.vertices.size() + i;
			int endpointB = a.edges[j][1] * b.vertices.size() + i;
			prod.edges.push_back({ endpointA, endpointB });
		}
	}

	//NEW FACES

	//a.edges x b.edges
	for (int i = 0; i < a.edges.size(); i++) {
		int aStart = a.edges[i][0];
		int aEnd = a.edges[i][1];
		for (int j = 0; j < b.edges.size(); j++) {
			std::vector<int> newFace;
			int bStart = b.edges[j][0];
			int bEnd = b.edges[j][1];
			newFace.push_back(aStart * b.vertices.size() + bStart);
			newFace.push_back(aStart * b.vertices.size() + bEnd);
			newFace.push_back(aEnd * b.vertices.size() + bStart);
			newFace.push_back(aEnd * b.vertices.size() + bEnd);

			prod.faces.push_back(newFace);
		}
	}

	//a.vertices x b.faces
	for (int i = 0; i < a.vertices.size(); i++) {
		for (int j = 0; j < b.faces.size(); j++) {
			std::vector<int> newFace;
			for (int k = 0; k < b.faces[j].size(); k++) {
				newFace.push_back(i * b.vertices.size() + b.faces[j][k]);
			}

			prod.faces.push_back(newFace);
		}
	}

	//a.faces x b.vertices
	for (int i = 0; i < b.vertices.size(); i++) {
		for (int j = 0; j < a.faces.size(); j++) {
			std::vector<int> newFace;
			for (int k = 0; k < a.faces[j].size(); k++) {
				newFace.push_back(a.faces[j][k] * b.vertices.size() + i);
			}

			prod.faces.push_back(newFace);
		}
	}

	//NEW VOLUMES

	//a.faces * b.edges
	for (int i = 0; i < a.faces.size(); i++) {
		for (int j = 0; j < b.edges.size(); j++) {
			std::vector<int> newVolume;
			for (int k = 0; k < a.faces[i].size(); k++) {
				newVolume.push_back(a.faces[i][k] * b.vertices.size() + b.edges[j][0]);
				newVolume.push_back(a.faces[i][k] * b.vertices.size() + b.edges[j][1]);
			}
			prod.volumes.push_back(newVolume);
		}
	}

	//a.edges * b.faces
	for (int i = 0; i < b.faces.size(); i++) {
		for (int j = 0; j < a.edges.size(); j++) {
			std::vector<int> newVolume;
			for (int k = 0; k < b.faces[i].size(); k++) {
				newVolume.push_back(a.edges[j][0] * b.vertices.size() +b.faces[i][k]);
				newVolume.push_back(a.edges[j][1] * b.vertices.size() + b.faces[i][k]);
			}
			prod.volumes.push_back(newVolume);
		}
	}


	return prod;
}

//Converting Polytope to Vertex Data
/*
The general idea is that the volume components of the polytope must be tetrahedralized. The problem is finding the most efficient way to do so.

First, it helps to make some key observations about the resulting volumes:
	a) All volume components come from (face X edge) operations
	b) Therefore, all volume components are prisms (extrusions of polygons)

Instead of creating a general-purpose tetrahedralization algorithm, which will likely be less efficient, we can use this observation to create a more specific algorithm

We can make an additional constraint that all faces are either triangular or rectangular. Now, there are only two cases to tetrahedralize, which can be done explicitly.
The tetrahedralization of a rectangular prism can be done with 5 tetrahedra.
The tetrahedralization of a triangular prism can be done with 8 tetrahedra

Now we need to take a step back and figure out how to process the Polytope to get the info we need (prism type, structure of points on the prism).
The type can be obtained from the number of points (8 for rectangular, 6 for triangular)

*/

std::vector<glm::vec4> GeometryGenerator::tetrahedralizePolytope(Polytope p) {
	for (int i = 0; i < p.volumes.size(); i++) {
		int type;
		switch (p.volumes[i].size()) {
		case 6:
			type = 0;
			break;
		case 8:
			type = 1;
			break;
		default:
			type = -1;
		}

		if (type == 0) {
			std::vector<int> mapped;

			/*
			0, 1
			1, 2
			2, 0
			3, 4
			4, 5
			5, 3
			0, 3
			1, 4
			2, 5
			*/
		}
		else if (type == 1) {
			std::vector<int> mapped;

			/*
			0, 1
			1, 2
			2, 3
			3, 0
			4, 5
			5, 6
			6, 7
			7, 4
			0, 4
			1, 5
			2, 6
			3, 7
			*/
		}
		else {
			return {};
		}

	}
}

void GeometryGenerator::printPolytope(Polytope p) {
	std::cout << "Vertices:" << std::endl;
	for (int i = 0; i < p.vertices.size(); i++) {
		std::cout << "(" << p.vertices[i].x << ", " << p.vertices[i].y << ", " << p.vertices[i].z << ", " << p.vertices[i].w << ")" << std::endl;
	}

	std::cout << "Edges:" << std::endl;
	for (int i = 0; i < p.edges.size(); i++) {
		std::cout << "{" << p.edges[i][0] << ", " << p.edges[i][1] << "}" << std::endl;
	}

	std::cout << "Faces:" << std::endl;
	for (int i = 0; i < p.faces.size(); i++) {
		std::cout << "{";
		for (int j = 0; j < p.faces[i].size(); j++) {
			std::cout << p.faces[i][j] << " ";
		}
		std::cout << "}" << std::endl;
	}

	std::cout << "Volumes:" << std::endl;
	for (int i = 0; i < p.volumes.size(); i++) {
		std::cout << "{";
		for (int j = 0; j < p.volumes[i].size(); j++) {
			std::cout << p.volumes[i][j] << " ";
		}
		std::cout << "}" << std::endl;
	}
}