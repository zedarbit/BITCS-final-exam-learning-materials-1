import pandas as pd
i = 0
def myfunc(x):
    global i
    i += 1
    print('{:09}/{:09}'.format(i, df.shape[0]), end='\r')
    res = x.split(',')[0][1:]
    try:
        pd.to_datetime(res)
    except Exception as e:
        print(res)
    return res
df = pd.read_csv('data/quaterfinal_gy_cmp_training_traveltime.txt', delimiter=';', dtype={'link_ID': object})
print(df.head(5))
temp = df['time_interval'].map(myfunc)
df['time_interval_begin'] = pd.to_datetime(temp)
pd.to_datetime('2017-05-06 12:04:00')
print(df.head(5))