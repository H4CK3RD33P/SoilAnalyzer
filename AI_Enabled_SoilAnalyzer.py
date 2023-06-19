import pandas as pd
import numpy as np
import warnings
from micromlgen import port
warnings.filterwarnings('ignore')
df = pd.read_csv('data.csv')
df.head()
x = df.drop(['crop'] , axis = 1)
y = df['crop']
from sklearn.model_selection import train_test_split
x_train, x_test, y_train, y_test = train_test_split(x,y, test_size = 0.3, random_state = 0)
from sklearn.svm import SVC
svc = SVC(probability=True,gamma=0.001,kernel='linear') #hyperparameter tuning 
svc.fit(x_train,y_train)
svc.score(x_train,y_train)
svcpredictions = svc.predict(np.array([[19,58,67],
                                       [24,45,80],
                                       [34,80,77]]))
print(port(svc))




