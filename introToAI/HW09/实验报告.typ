#import "simplepaper.typ": *
#show: project.with(
  title: "房价预测实验报告",
  authors: (
    (
      name: "林乐天 2300012154",
      email: "2300012154@stu.pku.edu.cn",
      organization: "北京大学生命科学学院"
    ),
  ),
)
== 背景介绍

房价预测是回归分析的一个经典任务，通过建立一个模型，使用多种度量方法和建模技法，
建立一个通过某房子的各种参数来预测该房屋出售时的价格。
本项目使用课程提供的数据集（未收集更多数据），使用python的sklearn库来建立模型，用多种数学建模处理技巧，得到了一个很可观的模型。

需要指出的是，除了得到一个最终模型之外，本项目还使用多种建模方法进行了探索，在下文中也会相应的指出。

== 数据的探索性分析
=== 整体数据格式
下面是表格
#image("images/2024-06-20_18-34-34_1718879674.png")
#image("images/2024-06-20_18-35-10_1718879710.png")

=== 各参数特色
可以看到各个列的属性：APARTMENT NUMBER的问题在于有大量缺失数据，且在数据字典里面没有这一项，猜测是门牌号。
门牌号数据及其复杂，理论上对房价有关系，但过于难以处理，于是放弃。

Land Square Feet和Gross Square Feet的数据也有问题，有-数据和0数据，面积为0显然是不合理的，应该是数据缺失。
那如何处理呢？一种方法是删除。根据统计，这种数据数量很多，不能一删了之。于是用中位数插值，得到新数据。
关于中位数插值的合理性，我们不妨认为房屋的大小近似符合高斯分布，用中位数/平均值来代替，便于处理。

Neighborhood等项是object类型，无法量化，对于这种项，baseline是用one hot编码来处理，会带来及其大量的数据，由于很多项的种类很多，
这样处理带来及其大量的变量，最终导致内存超限，分析耗时极长，最终放弃了这种方法。选择LabelEncoder来处理。

=== 数据相关性分析
为了提升模型表示效果，对非encoder的变量做正则化@dubin1998predicting。在此之后做VIF分析，得到的结果如下：
```
标准化后的 VIF 数据:
                                    feature         VIF
0                          BOROUGH_encoded    6.461019
1                          ADDRESS_encoded    3.517541
2                 APARTMENT NUMBER_encoded    2.651066
3          BUILDING CLASS CATEGORY_encoded   42.132857
4             TAX CLASS AT PRESENT_encoded   45.722047
5        TAX CLASS AT TIME OF SALE_encoded   57.634406
6                     NEIGHBORHOOD_encoded    3.312020
7        BUILDING CLASS AT PRESENT_encoded   60.238048
8                         ZIP CODE_encoded   10.398681
9                            BLOCK_encoded    5.129682
10  BUILDING CLASS AT TIME OF SALE_encoded   58.710357
11                                     LOT    2.409707
12                       RESIDENTIAL UNITS  253.324274
13                        COMMERCIAL UNITS   16.116134
14                             TOTAL UNITS  272.574196
15                        LAND SQUARE FEET    1.186716
16                       GROSS SQUARE FEET    2.119414
17                              YEAR BUILT    1.332734
18                                    year    3.151629
19                                   month    3.140644
20                                     day    1.002836
```
可以看到有非常严重的共线性性问题，需要降维。
== 特征工程
先试着用PCA来降维。

```python
#PCA
# 定义 PCA 并拟合
pca = PCA(n_components=20)  # 选择适当的组件数量
X_pca = pca.fit_transform(standardized_df)

train_X = X_pca[:num_train_samples]
test_X = X_pca[num_train_samples:]

rf_regr.fit(train_X, train_y)
Y_pred_training_rf = rf_regr.predict(train_X)
Y_pred_rf = rf_regr.predict(test_X)

# MAPE metric
print(mean_absolute_percentage_error(train_y,Y_pred_training_rf))
print(mean_absolute_percentage_error(test_y,Y_pred_rf))
```
输出：
```
0.14976295375570156
0.39576930865128224
```
还不如原先的方法

换一种方法：用随机森林选择变量，更客观剔除变量
```python
from sklearn.feature_selection import SelectFromModel

selector = SelectFromModel(RandomForestRegressor(n_estimators=100))
selector.fit(train_X, train_y)

# 获取重要特征
train_X_selected = selector.transform(train_X)
test_X_selected = selector.transform(test_X)

rf_regr = RandomForestRegressor()
rf_regr.fit(train_X_selected, train_y)
Y_pred_training_rf = rf_regr.predict(train_X_selected)
Y_pred_rf = rf_regr.predict(test_X_selected)

# MAPE metric
print(mean_absolute_percentage_error(train_y,Y_pred_training_rf))
print(mean_absolute_percentage_error(test_y,Y_pred_rf))
```
输出
```
0.1522343479774094
0.40488195100164365
```
效果不好，参数筛选过于随便，且共线性性依然不是很好。

采用更加综合的方法，考虑结合多种降维方法，尝试如下
```python
# 1. 初步特征选择（过滤法）
selector_var = VarianceThreshold(threshold=0.1)
X_train_var = selector_var.fit_transform(train_X)
X_test_var = selector_var.transform(test_X)

# 2. 特征重要性分析（嵌入法）
rf = RandomForestRegressor()
rf.fit(X_train_var, train_y)

importances = rf.feature_importances_
indices = importances.argsort()[-10:][::-1]
X_train_important = X_train_var[:, indices]
X_test_important = X_test_var[:, indices]

# 3. 高级特征选择（包装法）
estimator = RandomForestRegressor()
selector_rfe = RFE(estimator, n_features_to_select=5, step=1)
selector_rfe.fit(X_train_important, train_y)

X_train_final = selector_rfe.transform(X_train_important)
X_test_final = selector_rfe.transform(X_test_important)
rf_regr = RandomForestRegressor()
rf_regr.fit(X_train_final, train_y)
Y_pred_training_rf = rf_regr.predict(X_train_final)
Y_pred_rf = rf_regr.predict(X_test_final)

# MAPE metric
print(mean_absolute_percentage_error(train_y,Y_pred_training_rf))
print(mean_absolute_percentage_error(test_y,Y_pred_rf))
```
输出
```
0.14346510411684749
0.38379399899800765
```
相对于上两个有所提升。
== 预测模型的建立
做超参数调优，用`GridSearchCV`实现
```python
# 4. 超参数调优
param_grid = {
    'n_estimators': [100, 200,300,400],
    'max_depth': [10, 20,30,40],
    'min_samples_split': [2, 5,10,15],

    'min_samples_leaf': [1, 2,3,4]
}

grid_search = GridSearchCV(estimator=RandomForestRegressor(), param_grid=param_grid, cv=5, n_jobs=-1, verbose=2)
grid_search.fit(X_train_final, train_y)

best_rf = grid_search.best_estimator_
# 5. 评估
y_pred = best_rf.predict(X_test_final)
mse = mean_absolute_percentage_error(y_pred, test_y)

print(f"{mse}")
print(f"Best parameters:{best_rf} ")
```
输出：
```
0.2952960219186069
Best parameters:RandomForestRegressor(max_depth=20, min_samples_split=10, n_estimators=400)
```

达到了要求。效果不错，这证明了超参数调优对程序预测效率影响很大。

== 结果分析与总结

本人也做了其他模型的实验，如线性模型、Lasson Regression等，效果都不是很理想， MAPE 维持在0.8左右，这体现出随机森林算法表示性很不错，是做预测的一个很好的选择。

然而，随机森林算法对超参数比较敏感，需要仔细调参才能获得一个不错的结果；随机森林的运行效率远远低于其他几种方法，在实际应用时可能需要多线程操作等来加速训练推理。

== 致谢

感谢#link("https://github.com/1bitbool/SimplePaper/tree/main")[SimplePaper] 提供的论文模板。

感谢 Typst 项目组提供的编辑引擎

#bibliography("bibs.bib",style: "institute-of-electrical-and-electronics-engineers")
