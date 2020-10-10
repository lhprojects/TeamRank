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
	double p;
	double pa = WinGame(a, b);
	double pb = 1 - pa;
	if (std::max(winA, winB) == 1) {
		if (winA == 1) return pa;
		else return pb;
	} if (std::max(winA, winB) == 2) { // bo3
		if (winA == 2 && winB == 0) {
			p = pa * pa;
		} else if (winA == 2 && winB == 1) {
			p = 2 * pa * (1 - pa)*pa;
		} else if (winA == 1 && winB == 2) {
			p = 2 * (1 - pa) * pa * (1 - pa);
		} else if (winA == 0 && winB == 2) {
			p = (1 - pa) * (1 - pa);
		}
		return p;
	} else if (std::max(winA, winB) == 3) { // bo5

		if (winA == 3 && winB == 0) {
			p = pa * pa * pa;
		} else if (winA == 3 && winB == 1) {
			p = 3 * pb*pa*pa*pa;
		} else if (winA == 3 && winB == 2) {
			p = 6 * pb*pb*pa*pa*pa;
		} else if (winA == 2 && winB == 3) {
			p = 6 * pb*pb*pb*pa*pa;
		} else if (winA == 1 && winB == 3) {
			p = 3 * pb*pb*pb*pa;
		} else if (winA == 0 && winB == 3) {
			p = pb*pb*pb;
		}
	} else {
		printf("support only bo1, bo3 or bo5, you have a bo%d\n", std::max(winA, winB)*2+1);
		exit(1);
	}
	return p;
}

double WinMatchInclusively(Team const a, Team const b,
	int boN)
{
	if (boN == 1) {
		return WinMatch(a, b, 1, 0);
	} else if (boN == 3) {
		return WinMatch(a, b, 2, 0) + WinMatch(a, b, 2, 1);
	} else if (boN == 5) {
		return WinMatch(a, b, 3, 0) + WinMatch(a, b, 3, 1) + WinMatch(a, b, 3, 2);
	} else {
		printf("support bo3 or bo5\n");
		exit(1);
	}
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
	printf("\n\n");
}

double pvalue(Match &match) {
	using namespace std;
	int N = max(match.fWinA, match.fWinB);
	int boN = 1+2*max(match.fWinA, match.fWinB);
	std::vector<double> ps;
	for(int i = 0; i < N; ++i) {
		ps.push_back( WinMatch(match.fTeamA, match.fTeamB, i, N ) );	
		ps.push_back( WinMatch(match.fTeamA, match.fTeamB, N, i ) );	
		//printf("%d %f %d %d\n", i, ps[i], i, N);
	}
	double p = WinMatch(match.fTeamA, match.fTeamB, match.fWinA, match.fWinB);
	std::sort(ps.begin(), ps.end());
	double pvalue = 0;
	for(size_t i = 0; i < ps.size(); ++i) {
		//printf("%d %f\n", i, ps[i]);
		if(p < ps[i]-1E-6) {
			break;
		}
		pvalue += ps[i];
	}
	return pvalue;
}

void AnomalyMatches(Teams &teams, std::vector<Match> &matchs) {

	std::vector<double> ps;
	std::vector<int> index;
	for (auto &match : matchs) {
		ps.push_back(pvalue(match));
		index.push_back(index.size());
	}

	std::sort(index.begin(), index.end(), [&](int a, int b) {
		return ps[a] < ps[b];
	});

	printf("Anamoly Matches (Weak team beat strong team.):\n");
	for (auto &idx : index) {
		if (ps[idx] > 0.15) break;
		
		Match &mat = matchs[idx];
		printf("%5s %d:%d %5s (p-value = %6.2g)\n", mat.fTeamA.fName.c_str(), mat.fWinA,
			mat.fWinB, mat.fTeamB.fName.c_str(), ps[idx]);
	}

	ps.clear();
	index.clear();
	for (auto &match : matchs) {
		ps.push_back(WinMatch(match.fTeamA, match.fTeamB, match.fWinA, match.fWinB));
		index.push_back(index.size());
	}
	std::sort(index.begin(), index.end(), [&](int a, int b) {
		return ps[a] > ps[b];
	});
	printf("Most Likely Matches (Strong team beat weak team):\n");
	for (auto &idx : index) {
		if (ps[idx] < 0.7) break;

		Match &mat = matchs[idx];
		printf("%5s %d:%d %5s (prob. = %6.2g)\n", mat.fTeamA.fName.c_str(), mat.fWinA,
			mat.fWinB, mat.fTeamB.fName.c_str(), ps[idx]);
	}
	printf("\n\n");

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
		std::cout << t << " "<< std::ends;
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
			double p1 = WinMatchInclusively(t1, t2, 1);
			double p3 = WinMatchInclusively(t1, t2, 3);
			double p5 = WinMatchInclusively(t1, t2, 5);
			printf("%5s win %-5s = %6.3f%%(Bo1) %6.3f%%(Bo3) %6.3f%%(Bo5)\n",
				t1.fName.c_str(), t2.fName.c_str(),
				100*p1, 100 * p3, 100 * p5);
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
		int boN = 2 * std::max(wa, wb) - 1;
		expWinRates[idx] += WinMatchInclusively(ta, tb, boN);

	}
	printf("S.Diff. = Score difference\n");
	printf("P(Win.) = Probability to win calculated by score difference\n");
	printf("S.Diff. | %6s %6s WinRate | P(Win.)\n", "Matchs", "Win");
	for (size_t i = 0; i < ms.size(); ++i) {
		if (ms[i] != 0) {
			printf("%3d-%-3d | %6d %6d %6.1f%% | %6.1f%%\n",
				(int)i * gap, (int)i * gap + gap, ms[i], winMatchs[i], 100.*winMatchs[i] / ms[i], 100 * expWinRates[i] / ms[i]);
		}
		if (ms[i] == 0) {
			printf("%3d-%-3d | %6d %6d %6.1f%% | %6.1f%%\n",
				(int)i * gap, (int)i * gap + gap, ms[i], winMatchs[i], 100.*winMatchs[i] / 1, 100 * expWinRates[i] / 1);
		}
	}
	printf("\n\n");

}

// Gradient Decent
void Rank(Teams &teams, std::vector<Match> &matchs) {
	printf("Rank... Please wait a (few) second(s)\n");
	std::vector<double> Gradient(teams.fTeams.size());
	for (int it = 0; it < 500; ++it) {

		double b0 = LogP(matchs) + LogP_Pre(teams);
		for (size_t i = 0; i < teams.fTeams.size(); ++i) {
			auto &team = teams.fTeams[i];
			double s0 = team.fScore;
			team.fScore = s0 + 1E-3;
			double b1 = LogP(matchs) + LogP_Pre(teams);
			team.fScore = s0 - 1E-3;
			double b2 = LogP(matchs) + LogP_Pre(teams);
			Gradient[i] = (b2 - b1) / (2E-3);
			team.fScore = s0;
		}

		for (size_t i = 0; i < teams.fTeams.size(); ++i) {
			auto &team = teams.fTeams[i];
			double delta = 20 / sqrt(1.0 + 0.01*it);
			team.fScore += -delta*Gradient[i];
		}
	}
}

int main()
{

	Teams teams;
	std::vector<Match> matchs;

	ReadData(teams, matchs);
	WinLoseGames(teams, matchs);
	

	Rank(teams, matchs);
	std::vector<Team> rank(teams.fTeams.begin(), teams.fTeams.end());
	std::sort(rank.begin(), rank.end(), [](auto &x, auto &y) { return x.fScore > y.fScore; });	
	printf("Teams score:\n");
	for (auto &t : rank) {
		printf("%5s %6.3lf\n", t.fName.c_str(), t.fScore);
	}
	printf("In our model, single game win rate of A to B were 80%%, if the score of A is larger than B by 100.\n");
	printf("A Bo3 win rate of A to B were %4.1f%%, if the score of A is larger than B by 100.\n\n\n",
		100 * (0.8*0.8 + 2 * 0.8*0.2));



	ScoreToWinRate(teams, matchs);
	AnomalyMatches(teams, matchs);
	WinP(teams);

}

