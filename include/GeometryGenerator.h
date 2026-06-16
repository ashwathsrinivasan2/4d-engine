#ifndef GEOMETRY_GENERATOR
#define GEOMETRY_GENERATOR

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>



class GeometryGenerator {

	struct Polytope {
		std::vector<glm::vec4> vertices;
		std::vector<std::vector<int>> edges;
		std::vector<std::vector<int>> faces;
		std::vector<std::vector<int>> volumes;
		std::vector<std::vector<int>> hypervolumes;
	};

public:
	GeometryGenerator();
	Polytope product(Polytope, Polytope);
	void printPolytope(Polytope);
	std::vector<glm::vec4> tetrahedralizePolytope(Polytope);
};

#endif