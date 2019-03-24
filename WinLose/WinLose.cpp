#include <math.h>
#include <string> 
#include <vector>
#include <algorithm>
#include <map>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <set>

struct Team {
	Team(std::string name) : fName(name) { 
		fScore = 0;
		fWinGames = 0;
		fLoseGames = 0;
		fWinMatch = 0;
		fLoseMatch = 0;
	}
	std::string fName;
	double fScore;
	int fWinMatch;
	int fLoseMatch;
	int fWinGames;
	int fLoseGames;
};


struct Teams {

	void AddTeam(Team const &t)
	{
		fTeamsMap.insert(std::make_pair(t.fName, fTeams.size()));
		fTeams.push_back(t);
	}
	Team &FindTeam(std::string const&name)
	{
		auto it = fTeamsMap.find(name);
		if (it == fTeamsMap.end()) {
			printf("%s not found\n", name.c_str());
			exit(1);
		}
		return fTeams[it->second];
	}
	std::vector<Team> fTeams;
	std::map<std::string, int> fTeamsMap;
};


double WinGame(Team const a, Team const b)
{
	double d = a.fScore - b.fScore;
	double s = exp(d / 100* 1.3862943);
	double pa = s / (1. + s);
	return pa;
}

double WinMatch(Team const a, Team const b,
	int winA, int winB)
{
	double pa = WinGame(a, b);
	double p;
	if (winA == 2 && winB == 0) {
		p = pa*pa;
	} else if (winA == 2 && winB == 1) {
		p = 2 * pa * (1 - pa)*pa;
	} else if (winA == 1 && winB == 2) {
		p = 2*(1 - pa) * pa * (1 - pa);
	} else if (winA == 0 && winB == 2) {
		p = (1 - pa) * (1 - pa);
	}
	return p;
}

struct Match {
	Match(Teams &teams,
		char const *a,
		char const *b,
		int winA, int winB) : 
		fTeamA(teams.FindTeam(a)),
		fTeamB(teams.FindTeam(b))
	{
		fWinA = winA;
		fWinB = winB;
	}

	Match(
		Team &a,
		Team &b,
		int winA, int winB) :
		fTeamA(a),
		fTeamB(b)
	{
		fWinA = winA;
		fWinB = winB;
	}
	Team &fTeamA;
	Team &fTeamB;
	int fWinA;
	int fWinB;
};

double LogP(std::vector<Match> const &boNs)
{
	double logP = 0;
	for (auto &boN : boNs) {
		// assume bo3
		logP += log(WinMatch(boN.fTeamA, boN.fTeamB, boN.fWinA, boN.fWinB));
	}
	return logP;
}

double LogP_Pre(Teams &teams) {
	double logP = 0;
	for (auto &t : teams.fTeams) {
		// assume bo3
		logP += -t.fScore / 150 * t.fScore / 150;
	}
	return logP;
}

void WinLoseGames(Teams &teams, std::vector<Match> &matchs) {
	for (auto &m : matchs) {

		m.fTeamA.fWinGames += m.fWinA;
		m.fTeamB.fWinGames += m.fWinB;
		m.fTeamA.fLoseGames += m.fWinB;
		m.fTeamB.fLoseGames += m.fWinA;

		if (m.fWinA > m.fWinB) {
			m.fTeamA.fWinMatch += 1;
			m.fTeamB.fLoseMatch += 1;
		} else {
			m.fTeamA.fLoseMatch += 1;
			m.fTeamB.fWinMatch += 1;
		}
	}

	std::vector<Team> rank(teams.fTeams.begin(), teams.fTeams.end());
	std::sort(rank.begin(), rank.end(), [](Team &a, Team &b) {
		return a.fWinMatch > b.fWinMatch
			|| (a.fWinMatch == b.fWinMatch && a.fWinGames > b.fWinGames);
	});
	printf("Team    W-L(Match) W-L(Game)\n");
	for (auto &t : rank) {
		printf("%5s    %2d - %-2d       %2d - %-2d\n", t.fName.c_str(),
			t.fWinMatch, t.fLoseMatch, t.fWinGames, t.fLoseGames);
	}

}


void AnomalyMatches(Teams &teams, std::vector<Match> &matchs) {

	std::vector<double> ps;
	std::vector<int> index;
	for (auto &match : matchs) {
		Team t1("");
		double p0 = WinMatch(t1, t1, match.fWinA, match.fWinB);
		double p = WinMatch(match.fTeamA, match.fTeamB, match.fWinA, match.fWinB)/p0;
		ps.push_back(p);
		index.push_back(index.size());
	}

	std::sort(index.begin(), index.end(), [&](int a, int b) {
		return ps[a] < ps[b];
	});

	printf("Anamoly Matches (Weak team beat strong team.):\n");
	for (auto &idx : index) {
		if (ps[idx] > 0.5) break;
		
		Match &mat = matchs[idx];
		printf("%5s %d:%d %5s (Probabilit = %10.8f)\n", mat.fTeamA.fName.c_str(), mat.fWinA,
			mat.fWinB, mat.fTeamB.fName.c_str(), ps[idx]);
	}

	std::sort(index.begin(), index.end(), [&](int a, int b) {
		return ps[a] < ps[b];
	});

	std::sort(index.begin(), index.end(), [&](int a, int b) {
		return ps[a] > ps[b];
	});
	printf("Easy Matches (Strong team beat weak team):\n");
	for (auto &idx : index) {
		if (ps[idx] < 2) break;

		Match &mat = matchs[idx];
		printf("%5s %d:%d %5s (Probabilit = %10.8f)\n", mat.fTeamA.fName.c_str(), mat.fWinA,
			mat.fWinB, mat.fTeamB.fName.c_str(), ps[idx]);
	}

}

void ReadData(Teams &teams, std::vector<Match> &matchs) {

	std::vector<std::string> a;
	std::vector<std::string> b;
	std::vector<int> aW;
	std::vector<int> bW;

	{
		std::ifstream file;
		file.open("Match.txt");
		for (std::string line; std::getline(file, line);) {


			std::string line_copy = line;
			int c = std::count_if(line_copy.begin(), line_copy.end(), [](char c) {
				return c != ' ' || c != '\t' || c != '\n' || c != '\r';
			});
			int cs = std::count_if(line_copy.begin(), line_copy.end(), [](char c) {
				return c == '*';
			});

			if (c > 0 && cs == 0) {

				std::string an;
				std::string bn;
				int aw;
				int bw;
				std::istringstream ss(line);
				ss >> an >> aw >> bw >> bn;
				if (ss.fail()) {
					std::cout << "IO failed: " << line << std::endl;
				}
				else {
					a.push_back(an);
					b.push_back(bn);
					aW.push_back(aw);
					bW.push_back(bw);
				}
			}
		}
	}

	std::set<std::string> tns;
	for (auto &t : a) {
		tns.insert(t);
	}
	for (auto &t : b) {
		tns.insert(t);
	}

	std::cout << tns.size() << " teams:\n    ";
	for (auto &t : tns) {
		std::cout << t << std::ends;
		teams.AddTeam(Team(t));
	}

	printf("\n%d games:\n", (int)a.size());
	for (size_t i = 0; i < a.size(); ++i) {
		auto &t1 = teams.FindTeam(a[i]);
		auto &t2 = teams.FindTeam(b[i]);
		Match match(t1, t2, aW[i], bW[i]);
		matchs.push_back(match);
		printf("    %5s %d %d %5s\n", a[i].c_str(), aW[i], bW[i], b[i].c_str());
	}
}

void WinP(Teams &teams) {
	printf("Probability of winning match\n");
	for (auto &t1: teams.fTeams) {
		for (auto &t2 : teams.fTeams) {
			double p = WinMatch(t1, t2, 2, 1) + WinMatch(t1, t2, 2, 0);
			printf("%5s win %-5s = %6.3f%%\n", t1.fName.c_str(), t2.fName.c_str(), 100*p);
		}
	}
}

void ScoreToWinRate(Teams &teams, std::vector<Match> &matchs) {
	int gap = 20;
	int bins = 6;
	std::vector<int> ms(bins);
	std::vector<int> winMatchs(bins);
	std::vector<double> expWinRates(bins);

	for (auto &m : matchs) {
		auto ta = m.fTeamA;
		auto tb = m.fTeamB;
		auto wa = m.fWinA;
		auto wb = m.fWinB;
		if (ta.fScore < tb.fScore) {
			std::swap(ta, tb);
			std::swap(wa, wb);
		}

		double sd = ta.fScore - tb.fScore;
		int idx = (int)(sd / gap);
		if (idx > bins-1) idx = bins-1;
		ms[idx] += 1;
		winMatchs[idx] += wa > wb;
		expWinRates[idx] += WinMatch(ta, tb, 2, 0) + WinMatch(ta, tb, 2, 1);

	}
	printf("S.Diff. = Score difference\n");
	printf("P(Win.) = Probability to win calculated by score difference\n");
	printf("S.Diff. | Matchs  Win  Win./Matchs | P(Win.)\n");
	for (size_t i = 0; i < ms.size(); ++i) {
		if (ms[i] != 0) {
			printf("%3d-%-3d | %3d %3d %5.1f%% | %5.1f%%\n",
				i * gap, i * gap + gap, ms[i], winMatchs[i], 100.*winMatchs[i] / ms[i], 100 * expWinRates[i] / ms[i]);
		}
		if (ms[i] == 0) {
			printf("%3d-%-3d | %3d %3d %5.1f%% | %5.1f%%\n",
				i * gap, i * gap + gap, ms[i], winMatchs[i], 100.*winMatchs[i] / 1, 100 * expWinRates[i] / 1);
		}
	}
}

int main()
{

	Teams teams;
	std::vector<Match> matchs;

	ReadData(teams, matchs);
	WinLoseGames(teams, matchs);
	printf("Rank... Please wait a (few) second(s)\n");
	double lastB = LogP(matchs);
	for (int it = 0; it < 1000; ++it) {
		for (size_t i = 0; i < teams.fTeams.size(); ++i) {
			auto &team = teams.fTeams[i];
			double s = team.fScore;

			// increase 1
			double delta = 1 / sqrt(1 + 0.1*it);
			team.fScore = s + delta;
			double b = LogP(matchs) + LogP_Pre(teams);
			if (b > lastB) {
				lastB = b;
				//printf("%5s %6lf + 1 Likelihood %f\n", team.fName.c_str(), s, b);
			} else{
				team.fScore = s - delta;
				double b = LogP(matchs) + LogP_Pre(teams);
				if (b > lastB) {
					lastB = b;
					//printf("%5s %6lf - 1 Likelihood %f\n", team.fName.c_str(), s, b);
				} else {
					lastB = b;
					team.fScore = s;
					//printf("%5s %6lf + 0 Likelihood %f\n", team.fName.c_str(), s, b);
				}
			}
		}


		//printf("Iter %d\n", it);
		//printf("Teams rank:\n");
		//for (auto &t : rank) {
		//	printf("%5s %6.3lf\n", t.fName.c_str(), t.fScore);
		//}
	}
	std::vector<Team> rank(teams.fTeams.begin(), teams.fTeams.end());
	std::sort(rank.begin(), rank.end(), [](auto &x, auto &y) { return x.fScore > y.fScore; });
	printf("Teams score:\n");
	for (auto &t : rank) {
		printf("%5s %6.3lf\n", t.fName.c_str(), t.fScore);
	}
	printf("In our model, single game win rate of A to B were 80%%, if the score of A is larger than B by 100.\n");
	printf("A Bo3 win rate of A to B were %4.1f%%, if the score of A is larger than B by 100.\n",
		100*(0.8*0.8+2*0.8*0.2));

	ScoreToWinRate(teams, matchs);
	AnomalyMatches(teams, matchs);
	WinP(teams);

}
