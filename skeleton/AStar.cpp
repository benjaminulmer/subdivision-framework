#include "AStar.h"
#include <queue>
#include <set>
#include <vector>

typedef std::tuple<double, std::string> QueueT;
class AStarPriorityQueue
	: public std::priority_queue<
				QueueT,
				std::vector<QueueT>,
				std::greater<typename std::vector<QueueT>::value_type>
			>
{
	public:
		bool remove(const std::string value) {
			auto it = std::find_if(
				this->c.begin(),
				this->c.end(),
				[&value](auto &it) {
					return std::get<1>(it) == value;
				}
			);
			if (it != this->c.end()) {
				this->c.erase(it);
				std::make_heap(this->c.begin(), this->c.end(), this->comp);
				return true;
			} else {
				return false;
			}
		}
		unsigned int count(const std::string value) {
			unsigned int _count = 0;
			for (auto it : this->c) {
				if (std::get<1>(it) == value) {
					++_count;
				}
			}
			return _count;
		}
};

std::vector<std::string> AStar::reconstructPath(
	std::map<std::string, std::string> cameFrom,
	std::string current
) {
	std::vector<std::string> path;
	path.push_back(current);
	while (cameFrom.count(current) > 0) {
		current = cameFrom[current];
		path.push_back(current);
	}
	return path;
}
std::vector<std::string> AStar::findPath(
	SphCoord startCoord,
	SphCoord endCoord,
	ScoreFunction g,
	ScoreFunction h,
	double radius,
	double gridRadius,
	unsigned int level
) {
	typedef std::tuple<double, std::string> QueueT;
	std::set<std::string> closedSet;
	std::string start = SdogCell::codeForPos(
		startCoord.latitude,
		startCoord.longitude,
		radius,
		gridRadius,
		level
	);
	std::string goal = SdogCell::codeForPos(
		endCoord.latitude,
		endCoord.longitude,
		radius,
		gridRadius,
		level
	);

	AStarPriorityQueue openSet;
	openSet.push(std::make_tuple(0., start));

	std::map<std::string, std::string> cameFrom;

	std::map<std::string, double> gScore;
	gScore[start] = 0.;
	std::map<std::string, double> fScore;
	fScore[start] = h(start, goal);

	while (openSet.size() > 0) {
		double score;
		std::string current;
		std::tie(score, current) = openSet.top();
		if (current == goal) {
			return reconstructPath(cameFrom, current);
		}
		openSet.pop();
		closedSet.insert(current);

		std::vector<std::string> neighbours;
		SdogCell cell(current, gridRadius);
		cell.neighbours(neighbours);
		for(std::string neighbour : neighbours) {
			if (closedSet.count(neighbour) > 0) {
				continue;
			}
			double tentativeGScore = gScore[current] + g(current, neighbour);
			if(gScore.count(neighbour) > 0) {
				if (tentativeGScore >= gScore[neighbour]) {
					continue;
				}
			}
			cameFrom[neighbour] = current;
			gScore[neighbour] = tentativeGScore;
			double curFScore = fScore[neighbour] = gScore[neighbour] + h(neighbour, goal);
			if(openSet.count(neighbour) == 0) {
				openSet.push(std::make_tuple(curFScore, neighbour));
			}

		}
	}
	throw std::runtime_error("This shouldn't happen.");
}
