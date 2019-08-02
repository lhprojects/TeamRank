# TeamRank
根据历史比赛数据给队评分，再根据评分预测比赛结果

# Model
模型非常简单，如果A队对分数为a与B队分数为b，那么A队赢下B队的概率为：`1/exp(-(a-b)/138)`。
当`a-b == 100`时, A队对B队在一小局（Game）中战胜B队的概率为`80%`。
一场比赛（Match）可能有几局（Game）比赛。

实际上是OneHot + 逻辑回归。

# 数据输入
将数据文件[Match.txt](Match.txt)放在当前文件夹下。支持BO1（单场比赛），BO3（三局两胜），BO5（五局三胜）。

# 训练
梯度下降。手撸c++，实现很简单，梯度用差分计算。

# 输出

## 队伍已经比赛统计
```
16 teams:
    BLG EDG FPX IG JDG LGD OMG RNG RW SDG SN SS TOP V5 VG WE
103 games:
      TOP 0 2    IG
      FPX 2 0    RW
      ...
      RNG 1 2    IG
Team    W-L(Match) W-L(Game)
  FPX    10 - 2        22 - 10
   IG    10 - 3        22 - 7
  TOP    10 - 4        22 - 11
  JDG     9 - 4        19 - 15
  RNG     9 - 4        19 - 12
  EDG     8 - 5        18 - 12
  BLG     7 - 5        17 - 12
  SDG     7 - 6        17 - 14
   WE     7 - 5        14 - 14
   SN     6 - 8        16 - 19
   SS     5 - 9        14 - 22
   V5     4 - 9        12 - 19
   RW     4 - 9         9 - 20
  LGD     3 - 10       12 - 21
   VG     2 - 10       10 - 22
  OMG     2 - 10        8 - 21
```

## 队伍评分
```
Teams score:
   IG 77.333
  FPX 61.411
  TOP 47.567
  RNG 37.967
  EDG 24.954
  JDG 14.615
  BLG 13.504
  SDG 12.771
   WE  4.985
   SN -14.932
   SS -30.515
   V5 -37.770
  LGD -40.322
   RW -53.032
   VG -57.047
  OMG -61.488
In our model, single game win rate of A to B were 80%, if the score of A is larger than B by 100.
A Bo3 win rate of A to B were 96.0%, if the score of A is larger than B by 100.
```

### 模型检验
```
S.Diff. = Score difference
P(Win.) = Probability to win calculated by score difference
S.Diff. | Matchs  Win  Win./Matchs | P(Win.)
  0-20  |  22  15  68.2% |  55.6%
 20-40  |  22  18  81.8% |  64.8%
 40-60  |  18  14  77.8% |  73.4%
 60-80  |  21  17  81.0% |  81.7%
 80-100 |   8   7  87.5% |  87.0%
100-120 |  12  11  91.7% |  92.4%
 ```

## 异常比赛和极正常比赛
```
Anamoly Matches (Weak team beat strong team.):
   IG 0:2   LGD (Probabilit = 0.10717391)
  RNG 0:2    RW (Probabilit = 0.19485385)
  OMG 2:0   BLG (Probabilit = 0.27295440)
  EDG 0:2   LGD (Probabilit = 0.33186750)
   SN 2:0   RNG (Probabilit = 0.42109900)
  SDG 1:2    VG (Probabilit = 0.43940945)
   IG 1:2   JDG (Probabilit = 0.49178759)
Easy Matches (Strong team beat weak team):
   IG 2:0   OMG (Probabilit = 3.04597104)
   IG 2:0    VG (Probabilit = 2.99728628)
   IG 2:0    RW (Probabilit = 2.95172065)
  FPX 2:0   OMG (Probabilit = 2.86301599)
   IG 2:0    V5 (Probabilit = 2.76498472)
  FPX 2:0    RW (Probabilit = 2.75643560)
  TOP 2:0   OMG (Probabilit = 2.68522204)
   IG 2:0    SS (Probabilit = 2.66890742)
  TOP 2:0    VG (Probabilit = 2.62464623)
   RW 0:2   TOP (Probabilit = 2.56850192)
   VG 0:2   RNG (Probabilit = 2.48825952)
   IG 2:0    SN (Probabilit = 2.44791517)
  LGD 0:2   TOP (Probabilit = 2.38260143)
  EDG 2:0   OMG (Probabilit = 2.36070858)
   SS 0:2   TOP (Probabilit = 2.23177626)
   RW 0:2   EDG (Probabilit = 2.23028224)
  RNG 2:0    V5 (Probabilit = 2.19493256)
  OMG 0:2   SDG (Probabilit = 2.17155477)
   IG 2:0    WE (Probabilit = 2.14118589)
   VG 0:2   BLG (Probabilit = 2.11247904)
  RNG 2:0    SS (Probabilit = 2.07928715)
   RW 0:2   BLG (Probabilit = 2.04792666)
  SDG 2:0    RW (Probabilit = 2.03608298)
   IG 2:0   SDG (Probabilit = 2.01596518)
```
   
## 队伍交战胜率预测
```
Probability of winning match
  BLG win BLG   = 50.000%(Bo1) 50.000%(Bo3) 50.000%(Bo5)
  BLG win EDG   = 46.040%(Bo1) 44.072%(Bo3) 42.606%(Bo5)
  BLG win FPX   = 33.981%(Bo1) 26.794%(Bo3) 21.957%(Bo5)
  BLG win IG    = 29.217%(Bo1) 20.621%(Bo3) 15.288%(Bo5)
  BLG win JDG   = 49.615%(Bo1) 49.423%(Bo3) 49.278%(Bo5)
  BLG win LGD   = 67.835%(Bo1) 75.618%(Bo3) 80.712%(Bo5)
  BLG win OMG   = 73.877%(Bo1) 83.094%(Bo3) 88.429%(Bo5)
  BLG win RNG   = 41.602%(Bo1) 37.521%(Bo3) 34.547%(Bo5)
  BLG win RW    = 71.553%(Bo1) 80.327%(Bo3) 85.685%(Bo5)
  BLG win SDG   = 50.254%(Bo1) 50.381%(Bo3) 50.476%(Bo5)
  BLG win SN    = 59.730%(Bo1) 64.410%(Bo3) 67.788%(Bo5)
  BLG win SS    = 64.799%(Bo1) 71.551%(Bo3) 76.170%(Bo5)
  BLG win TOP   = 38.409%(Bo1) 32.925%(Bo3) 29.033%(Bo5)
  BLG win V5    = 67.058%(Bo1) 74.594%(Bo3) 79.589%(Bo5)
  BLG win VG    = 72.672%(Bo1) 81.677%(Bo3) 87.042%(Bo5)
  BLG win WE    = 52.949%(Bo1) 54.419%(Bo3) 55.517%(Bo5)
  EDG win BLG   = 53.960%(Bo1) 55.928%(Bo3) 57.394%(Bo5)
  EDG win EDG   = 50.000%(Bo1) 50.000%(Bo3) 50.000%(Bo5)
  EDG win FPX   = 37.627%(Bo1) 31.820%(Bo3) 27.731%(Bo5)
  EDG win IG    = 32.605%(Bo1) 24.960%(Bo3) 19.920%(Bo5)
  EDG win JDG   = 53.577%(Bo1) 55.357%(Bo3) 56.684%(Bo5)
  EDG win LGD   = 71.196%(Bo1) 79.889%(Bo3) 85.238%(Bo5)
  EDG win OMG   = 76.823%(Bo1) 86.375%(Bo3) 91.477%(Bo5)
  EDG win RNG   = 45.502%(Bo1) 43.271%(Bo3) 41.612%(Bo5)
  EDG win RW    = 74.671%(Bo1) 84.003%(Bo3) 89.298%(Bo5)
  EDG win SDG   = 54.212%(Bo1) 56.303%(Bo3) 57.861%(Bo5)
  EDG win SN    = 63.482%(Bo1) 69.733%(Bo3) 74.080%(Bo5)
  EDG win SS    = 68.330%(Bo1) 76.263%(Bo3) 81.413%(Bo5)
  EDG win TOP   = 42.226%(Bo1) 38.433%(Bo3) 35.658%(Bo5)
  EDG win V5    = 70.465%(Bo1) 78.983%(Bo3) 84.302%(Bo5)
  EDG win VG    = 75.709%(Bo1) 85.165%(Bo3) 90.382%(Bo5)
  EDG win WE    = 56.877%(Bo1) 60.250%(Bo3) 62.733%(Bo5)
  FPX win BLG   = 66.019%(Bo1) 73.206%(Bo3) 78.043%(Bo5)
  FPX win EDG   = 62.373%(Bo1) 68.180%(Bo3) 72.269%(Bo5)
  FPX win FPX   = 50.000%(Bo1) 50.000%(Bo3) 50.000%(Bo5)
  FPX win IG    = 44.504%(Bo1) 41.789%(Bo3) 39.778%(Bo5)
  FPX win JDG   = 65.672%(Bo1) 72.739%(Bo3) 77.518%(Bo5)
  FPX win LGD   = 80.382%(Bo1) 89.964%(Bo3) 94.497%(Bo5)
  FPX win OMG   = 84.602%(Bo1) 93.617%(Bo3) 97.141%(Bo5)
  FPX win RNG   = 58.054%(Bo1) 61.977%(Bo3) 64.842%(Bo5)
  FPX win RW    = 83.013%(Bo1) 92.323%(Bo3) 96.262%(Bo5)
  FPX win SDG   = 66.246%(Bo1) 73.512%(Bo3) 78.386%(Bo5)
  FPX win SN    = 74.237%(Bo1) 83.508%(Bo3) 88.828%(Bo5)
  FPX win SS    = 78.149%(Bo1) 87.762%(Bo3) 92.687%(Bo5)
  FPX win TOP   = 54.783%(Bo1) 57.153%(Bo3) 58.914%(Bo5)
  FPX win V5    = 79.818%(Bo1) 89.424%(Bo3) 94.067%(Bo5)
  FPX win VG    = 83.783%(Bo1) 92.963%(Bo3) 96.705%(Bo5)
  FPX win WE    = 68.616%(Bo1) 76.634%(Bo3) 81.814%(Bo5)
   IG win BLG   = 70.783%(Bo1) 79.379%(Bo3) 84.712%(Bo5)
   IG win EDG   = 67.395%(Bo1) 75.040%(Bo3) 80.080%(Bo5)
   IG win FPX   = 55.496%(Bo1) 58.211%(Bo3) 60.222%(Bo5)
   IG win IG    = 50.000%(Bo1) 50.000%(Bo3) 50.000%(Bo5)
   IG win JDG   = 70.463%(Bo1) 78.981%(Bo3) 84.299%(Bo5)
   IG win LGD   = 83.631%(Bo1) 92.839%(Bo3) 96.621%(Bo5)
   IG win OMG   = 87.264%(Bo1) 95.547%(Bo3) 98.309%(Bo5)
   IG win RNG   = 63.314%(Bo1) 69.499%(Bo3) 73.809%(Bo5)
   IG win RW    = 85.903%(Bo1) 94.598%(Bo3) 97.757%(Bo5)
   IG win SDG   = 70.992%(Bo1) 79.638%(Bo3) 84.980%(Bo5)
   IG win SN    = 78.229%(Bo1) 87.845%(Bo3) 92.757%(Bo5)
   IG win SS    = 81.684%(Bo1) 91.165%(Bo3) 95.420%(Bo5)
   IG win TOP   = 60.172%(Bo1) 65.047%(Bo3) 68.553%(Bo5)
   IG win V5    = 83.141%(Bo1) 92.432%(Bo3) 96.338%(Bo5)
   IG win VG    = 86.563%(Bo1) 95.069%(Bo3) 98.037%(Bo5)
   IG win WE    = 73.164%(Bo1) 82.260%(Bo3) 87.618%(Bo5)
  JDG win BLG   = 50.385%(Bo1) 50.577%(Bo3) 50.722%(Bo5)
  JDG win EDG   = 46.423%(Bo1) 44.643%(Bo3) 43.316%(Bo5)
  JDG win FPX   = 34.328%(Bo1) 27.261%(Bo3) 22.482%(Bo5)
  JDG win IG    = 29.537%(Bo1) 21.019%(Bo3) 15.701%(Bo5)
  JDG win JDG   = 50.000%(Bo1) 50.000%(Bo3) 50.000%(Bo5)
  JDG win LGD   = 68.170%(Bo1) 76.055%(Bo3) 81.188%(Bo5)
  JDG win OMG   = 74.174%(Bo1) 83.435%(Bo3) 88.758%(Bo5)
  JDG win RNG   = 41.977%(Bo1) 38.068%(Bo3) 35.212%(Bo5)
  JDG win RW    = 71.865%(Bo1) 80.707%(Bo3) 86.070%(Bo5)
  JDG win SDG   = 50.639%(Bo1) 50.958%(Bo3) 51.198%(Bo5)
  JDG win SN    = 60.099%(Bo1) 64.943%(Bo3) 68.428%(Bo5)
  JDG win SS    = 65.150%(Bo1) 72.029%(Bo3) 76.715%(Bo5)
  JDG win TOP   = 38.774%(Bo1) 33.444%(Bo3) 29.648%(Bo5)
  JDG win V5    = 67.397%(Bo1) 75.043%(Bo3) 80.083%(Bo5)
  JDG win VG    = 72.977%(Bo1) 82.039%(Bo3) 87.400%(Bo5)
  JDG win WE    = 53.333%(Bo1) 54.991%(Bo3) 56.230%(Bo5)
  LGD win BLG   = 32.165%(Bo1) 24.382%(Bo3) 19.288%(Bo5)
  LGD win EDG   = 28.804%(Bo1) 20.111%(Bo3) 14.762%(Bo5)
  LGD win FPX   = 19.618%(Bo1) 10.036%(Bo3)  5.503%(Bo5)
  LGD win IG    = 16.369%(Bo1)  7.161%(Bo3)  3.379%(Bo5)
  LGD win JDG   = 31.830%(Bo1) 23.945%(Bo3) 18.812%(Bo5)
  LGD win LGD   = 50.000%(Bo1) 50.000%(Bo3) 50.000%(Bo5)
  LGD win OMG   = 57.283%(Bo1) 60.848%(Bo3) 63.464%(Bo5)
  LGD win RNG   = 25.250%(Bo1) 15.907%(Bo3) 10.617%(Bo5)
  LGD win RW    = 54.394%(Bo1) 56.573%(Bo3) 58.196%(Bo5)
  LGD win SDG   = 32.387%(Bo1) 24.674%(Bo3) 19.606%(Bo5)
  LGD win SN    = 41.290%(Bo1) 37.067%(Bo3) 33.997%(Bo5)
  LGD win SS    = 46.606%(Bo1) 44.917%(Bo3) 43.656%(Bo5)
  LGD win TOP   = 22.822%(Bo1) 13.248%(Bo3)  8.189%(Bo5)
  LGD win V5    = 49.116%(Bo1) 48.674%(Bo3) 48.342%(Bo5)
  LGD win VG    = 55.770%(Bo1) 58.617%(Bo3) 60.724%(Bo5)
  LGD win WE    = 34.794%(Bo1) 27.895%(Bo3) 23.199%(Bo5)
  OMG win BLG   = 26.123%(Bo1) 16.906%(Bo3) 11.571%(Bo5)
  OMG win EDG   = 23.177%(Bo1) 13.625%(Bo3)  8.523%(Bo5)
  OMG win FPX   = 15.398%(Bo1)  6.383%(Bo3)  2.859%(Bo5)
  OMG win IG    = 12.736%(Bo1)  4.453%(Bo3)  1.691%(Bo5)
  OMG win JDG   = 25.826%(Bo1) 16.565%(Bo3) 11.242%(Bo5)
  OMG win LGD   = 42.717%(Bo1) 39.152%(Bo3) 36.536%(Bo5)
  OMG win OMG   = 50.000%(Bo1) 50.000%(Bo3) 50.000%(Bo5)
  OMG win RNG   = 20.121%(Bo1) 10.516%(Bo3)  5.885%(Bo5)
  OMG win RW    = 47.073%(Bo1) 45.614%(Bo3) 44.524%(Bo5)
  OMG win SDG   = 26.319%(Bo1) 17.135%(Bo3) 11.791%(Bo5)
  OMG win SN    = 34.403%(Bo1) 27.363%(Bo3) 22.597%(Bo5)
  OMG win SS    = 39.427%(Bo1) 34.377%(Bo3) 30.759%(Bo5)
  OMG win TOP   = 18.067%(Bo1)  8.613%(Bo3)  4.414%(Bo5)
  OMG win V5    = 41.853%(Bo1) 37.888%(Bo3) 34.993%(Bo5)
  OMG win VG    = 48.461%(Bo1) 47.692%(Bo3) 47.116%(Bo5)
  OMG win WE    = 28.465%(Bo1) 19.695%(Bo3) 14.338%(Bo5)
  RNG win BLG   = 58.398%(Bo1) 62.479%(Bo3) 65.453%(Bo5)
  RNG win EDG   = 54.498%(Bo1) 56.729%(Bo3) 58.388%(Bo5)
  RNG win FPX   = 41.946%(Bo1) 38.023%(Bo3) 35.158%(Bo5)
  RNG win IG    = 36.686%(Bo1) 30.501%(Bo3) 26.191%(Bo5)
  RNG win JDG   = 58.023%(Bo1) 61.932%(Bo3) 64.788%(Bo5)
  RNG win LGD   = 74.750%(Bo1) 84.093%(Bo3) 89.383%(Bo5)
  RNG win OMG   = 79.879%(Bo1) 89.484%(Bo3) 94.115%(Bo5)
  RNG win RNG   = 50.000%(Bo1) 50.000%(Bo3) 50.000%(Bo5)
  RNG win RW    = 77.929%(Bo1) 87.536%(Bo3) 92.494%(Bo5)
  RNG win SDG   = 58.645%(Bo1) 62.838%(Bo3) 65.889%(Bo5)
  RNG win SN    = 67.554%(Bo1) 75.249%(Bo3) 80.309%(Bo5)
  RNG win SS    = 72.099%(Bo1) 80.990%(Bo3) 86.355%(Bo5)
  RNG win TOP   = 46.678%(Bo1) 45.024%(Bo3) 43.789%(Bo5)
  RNG win V5    = 74.077%(Bo1) 83.323%(Bo3) 88.651%(Bo5)
  RNG win VG    = 78.871%(Bo1) 88.494%(Bo3) 93.304%(Bo5)
  RNG win WE    = 61.236%(Bo1) 66.570%(Bo3) 70.369%(Bo5)
   RW win BLG   = 28.447%(Bo1) 19.673%(Bo3) 14.315%(Bo5)
   RW win EDG   = 25.329%(Bo1) 15.997%(Bo3) 10.702%(Bo5)
   RW win FPX   = 16.987%(Bo1)  7.677%(Bo3)  3.738%(Bo5)
   RW win IG    = 14.097%(Bo1)  5.402%(Bo3)  2.243%(Bo5)
   RW win JDG   = 28.135%(Bo1) 19.293%(Bo3) 13.930%(Bo5)
   RW win LGD   = 45.606%(Bo1) 43.427%(Bo3) 41.804%(Bo5)
   RW win OMG   = 52.927%(Bo1) 54.386%(Bo3) 55.476%(Bo5)
   RW win RNG   = 22.071%(Bo1) 12.464%(Bo3)  7.506%(Bo5)
   RW win RW    = 50.000%(Bo1) 50.000%(Bo3) 50.000%(Bo5)
   RW win SDG   = 28.654%(Bo1) 19.927%(Bo3) 14.574%(Bo5)
   RW win SN    = 37.094%(Bo1) 31.071%(Bo3) 26.855%(Bo5)
   RW win SS    = 42.259%(Bo1) 38.481%(Bo3) 35.715%(Bo5)
   RW win TOP   = 19.867%(Bo1) 10.273%(Bo3)  5.691%(Bo5)
   RW win V5    = 44.730%(Bo1) 42.125%(Bo3) 40.192%(Bo5)
   RW win VG    = 51.391%(Bo1) 52.086%(Bo3) 52.606%(Bo5)
   RW win WE    = 30.911%(Bo1) 22.758%(Bo3) 17.534%(Bo5)
  SDG win BLG   = 49.746%(Bo1) 49.619%(Bo3) 49.524%(Bo5)
  SDG win EDG   = 45.788%(Bo1) 43.697%(Bo3) 42.139%(Bo5)
  SDG win FPX   = 33.754%(Bo1) 26.488%(Bo3) 21.614%(Bo5)
  SDG win IG    = 29.008%(Bo1) 20.362%(Bo3) 15.020%(Bo5)
  SDG win JDG   = 49.361%(Bo1) 49.042%(Bo3) 48.802%(Bo5)
  SDG win LGD   = 67.613%(Bo1) 75.326%(Bo3) 80.394%(Bo5)
  SDG win OMG   = 73.681%(Bo1) 82.865%(Bo3) 88.209%(Bo5)
  SDG win RNG   = 41.355%(Bo1) 37.162%(Bo3) 34.111%(Bo5)
  SDG win RW    = 71.346%(Bo1) 80.073%(Bo3) 85.426%(Bo5)
  SDG win SDG   = 50.000%(Bo1) 50.000%(Bo3) 50.000%(Bo5)
  SDG win SN    = 59.485%(Bo1) 64.057%(Bo3) 67.362%(Bo5)
  SDG win SS    = 64.567%(Bo1) 71.233%(Bo3) 75.807%(Bo5)
  SDG win TOP   = 38.169%(Bo1) 32.585%(Bo3) 28.631%(Bo5)
  SDG win V5    = 66.833%(Bo1) 74.296%(Bo3) 79.259%(Bo5)
  SDG win VG    = 72.470%(Bo1) 81.436%(Bo3) 86.802%(Bo5)
  SDG win WE    = 52.696%(Bo1) 54.040%(Bo3) 55.045%(Bo5)
   SN win BLG   = 40.270%(Bo1) 35.590%(Bo3) 32.212%(Bo5)
   SN win EDG   = 36.518%(Bo1) 30.267%(Bo3) 25.920%(Bo5)
   SN win FPX   = 25.763%(Bo1) 16.492%(Bo3) 11.172%(Bo5)
   SN win IG    = 21.771%(Bo1) 12.155%(Bo3)  7.243%(Bo5)
   SN win JDG   = 39.901%(Bo1) 35.057%(Bo3) 31.572%(Bo5)
   SN win LGD   = 58.710%(Bo1) 62.933%(Bo3) 66.003%(Bo5)
   SN win OMG   = 65.597%(Bo1) 72.637%(Bo3) 77.403%(Bo5)
   SN win RNG   = 32.446%(Bo1) 24.751%(Bo3) 19.691%(Bo5)
   SN win RW    = 62.906%(Bo1) 68.929%(Bo3) 73.145%(Bo5)
   SN win SDG   = 40.515%(Bo1) 35.943%(Bo3) 32.638%(Bo5)
   SN win SN    = 50.000%(Bo1) 50.000%(Bo3) 50.000%(Bo5)
   SN win SS    = 55.379%(Bo1) 58.038%(Bo3) 60.009%(Bo5)
   SN win TOP   = 29.600%(Bo1) 21.098%(Bo3) 15.783%(Bo5)
   SN win V5    = 57.850%(Bo1) 61.678%(Bo3) 64.478%(Bo5)
   SN win VG    = 64.195%(Bo1) 70.720%(Bo3) 75.220%(Bo5)
   SN win WE    = 43.141%(Bo1) 39.776%(Bo3) 37.299%(Bo5)
   SS win BLG   = 35.201%(Bo1) 28.449%(Bo3) 23.830%(Bo5)
   SS win EDG   = 31.670%(Bo1) 23.737%(Bo3) 18.587%(Bo5)
   SS win FPX   = 21.851%(Bo1) 12.238%(Bo3)  7.313%(Bo5)
   SS win IG    = 18.316%(Bo1)  8.835%(Bo3)  4.580%(Bo5)
   SS win JDG   = 34.850%(Bo1) 27.971%(Bo3) 23.285%(Bo5)
   SS win LGD   = 53.394%(Bo1) 55.083%(Bo3) 56.344%(Bo5)
   SS win OMG   = 60.573%(Bo1) 65.623%(Bo3) 69.241%(Bo5)
   SS win RNG   = 27.901%(Bo1) 19.010%(Bo3) 13.645%(Bo5)
   SS win RW    = 57.741%(Bo1) 61.519%(Bo3) 64.285%(Bo5)
   SS win SDG   = 35.433%(Bo1) 28.767%(Bo3) 24.193%(Bo5)
   SS win SN    = 44.621%(Bo1) 41.962%(Bo3) 39.991%(Bo5)
   SS win SS    = 50.000%(Bo1) 50.000%(Bo3) 50.000%(Bo5)
   SS win TOP   = 25.304%(Bo1) 15.969%(Bo3) 10.675%(Bo5)
   SS win V5    = 52.513%(Bo1) 53.766%(Bo3) 54.703%(Bo5)
   SS win VG    = 59.093%(Bo1) 63.489%(Bo3) 66.677%(Bo5)
   SS win WE    = 37.939%(Bo1) 32.260%(Bo3) 28.248%(Bo5)
  TOP win BLG   = 61.591%(Bo1) 67.075%(Bo3) 70.967%(Bo5)
  TOP win EDG   = 57.774%(Bo1) 61.567%(Bo3) 64.342%(Bo5)
  TOP win FPX   = 45.217%(Bo1) 42.847%(Bo3) 41.086%(Bo5)
  TOP win IG    = 39.828%(Bo1) 34.953%(Bo3) 31.447%(Bo5)
  TOP win JDG   = 61.226%(Bo1) 66.556%(Bo3) 70.352%(Bo5)
  TOP win LGD   = 77.178%(Bo1) 86.752%(Bo3) 91.811%(Bo5)
  TOP win OMG   = 81.933%(Bo1) 91.387%(Bo3) 95.586%(Bo5)
  TOP win RNG   = 53.322%(Bo1) 54.976%(Bo3) 56.211%(Bo5)
  TOP win RW    = 80.133%(Bo1) 89.727%(Bo3) 94.309%(Bo5)
  TOP win SDG   = 61.831%(Bo1) 67.415%(Bo3) 71.369%(Bo5)
  TOP win SN    = 70.400%(Bo1) 78.902%(Bo3) 84.217%(Bo5)
  TOP win SS    = 74.696%(Bo1) 84.031%(Bo3) 89.325%(Bo5)
  TOP win TOP   = 50.000%(Bo1) 50.000%(Bo3) 50.000%(Bo5)
  TOP win V5    = 76.549%(Bo1) 86.081%(Bo3) 91.215%(Bo5)
  TOP win VG    = 81.004%(Bo1) 90.545%(Bo3) 94.950%(Bo5)
  TOP win WE    = 64.344%(Bo1) 70.926%(Bo3) 75.456%(Bo5)
   V5 win BLG   = 32.942%(Bo1) 25.406%(Bo3) 20.411%(Bo5)
   V5 win EDG   = 29.535%(Bo1) 21.017%(Bo3) 15.698%(Bo5)
   V5 win FPX   = 20.182%(Bo1) 10.576%(Bo3)  5.933%(Bo5)
   V5 win IG    = 16.859%(Bo1)  7.568%(Bo3)  3.662%(Bo5)
   V5 win JDG   = 32.603%(Bo1) 24.957%(Bo3) 19.917%(Bo5)
   V5 win LGD   = 50.884%(Bo1) 51.326%(Bo3) 51.658%(Bo5)
   V5 win OMG   = 58.147%(Bo1) 62.112%(Bo3) 65.007%(Bo5)
   V5 win RNG   = 25.923%(Bo1) 16.677%(Bo3) 11.349%(Bo5)
   V5 win RW    = 55.270%(Bo1) 57.875%(Bo3) 59.808%(Bo5)
   V5 win SDG   = 33.167%(Bo1) 25.704%(Bo3) 20.741%(Bo5)
   V5 win SN    = 42.150%(Bo1) 38.322%(Bo3) 35.522%(Bo5)
   V5 win SS    = 47.487%(Bo1) 46.234%(Bo3) 45.297%(Bo5)
   V5 win TOP   = 23.451%(Bo1) 13.919%(Bo3)  8.785%(Bo5)
   V5 win V5    = 50.000%(Bo1) 50.000%(Bo3) 50.000%(Bo5)
   V5 win VG    = 56.641%(Bo1) 59.903%(Bo3) 62.306%(Bo5)
   V5 win WE    = 35.601%(Bo1) 28.999%(Bo3) 24.458%(Bo5)
   VG win BLG   = 27.328%(Bo1) 18.323%(Bo3) 12.958%(Bo5)
   VG win EDG   = 24.291%(Bo1) 14.835%(Bo3)  9.618%(Bo5)
   VG win FPX   = 16.217%(Bo1)  7.037%(Bo3)  3.295%(Bo5)
   VG win IG    = 13.437%(Bo1)  4.931%(Bo3)  1.963%(Bo5)
   VG win JDG   = 27.023%(Bo1) 17.961%(Bo3) 12.600%(Bo5)
   VG win LGD   = 44.230%(Bo1) 41.383%(Bo3) 39.276%(Bo5)
   VG win OMG   = 51.539%(Bo1) 52.308%(Bo3) 52.884%(Bo5)
   VG win RNG   = 21.129%(Bo1) 11.506%(Bo3)  6.696%(Bo5)
   VG win RW    = 48.609%(Bo1) 47.914%(Bo3) 47.394%(Bo5)
   VG win SDG   = 27.530%(Bo1) 18.564%(Bo3) 13.198%(Bo5)
   VG win SN    = 35.805%(Bo1) 29.280%(Bo3) 24.780%(Bo5)
   VG win SS    = 40.907%(Bo1) 36.511%(Bo3) 33.323%(Bo5)
   VG win TOP   = 18.996%(Bo1)  9.455%(Bo3)  5.050%(Bo5)
   VG win V5    = 43.359%(Bo1) 40.097%(Bo3) 37.694%(Bo5)
   VG win VG    = 50.000%(Bo1) 50.000%(Bo3) 50.000%(Bo5)
   VG win WE    = 29.735%(Bo1) 21.267%(Bo3) 15.960%(Bo5)
   WE win BLG   = 47.051%(Bo1) 45.581%(Bo3) 44.483%(Bo5)
   WE win EDG   = 43.123%(Bo1) 39.750%(Bo3) 37.267%(Bo5)
   WE win FPX   = 31.384%(Bo1) 23.366%(Bo3) 18.186%(Bo5)
   WE win IG    = 26.836%(Bo1) 17.740%(Bo3) 12.382%(Bo5)
   WE win JDG   = 46.667%(Bo1) 45.009%(Bo3) 43.770%(Bo5)
   WE win LGD   = 65.206%(Bo1) 72.105%(Bo3) 76.801%(Bo5)
   WE win OMG   = 71.535%(Bo1) 80.305%(Bo3) 85.662%(Bo5)
   WE win RNG   = 38.764%(Bo1) 33.430%(Bo3) 29.631%(Bo5)
   WE win RW    = 69.089%(Bo1) 77.242%(Bo3) 82.466%(Bo5)
   WE win SDG   = 47.304%(Bo1) 45.960%(Bo3) 44.955%(Bo5)
   WE win SN    = 56.859%(Bo1) 60.224%(Bo3) 62.701%(Bo5)
   WE win SS    = 62.061%(Bo1) 67.740%(Bo3) 71.752%(Bo5)
   WE win TOP   = 35.656%(Bo1) 29.074%(Bo3) 24.544%(Bo5)
   WE win V5    = 64.399%(Bo1) 71.001%(Bo3) 75.542%(Bo5)
   WE win VG    = 70.265%(Bo1) 78.733%(Bo3) 84.040%(Bo5)
   WE win WE    = 50.000%(Bo1) 50.000%(Bo3) 50.000%(Bo5)
```

