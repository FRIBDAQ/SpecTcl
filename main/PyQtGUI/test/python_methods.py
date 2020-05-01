import unittest
import pandas as pd
from pandas.util.testing import assert_frame_equal, assert_series_equal
import scipy as sy
from scipy.optimize import curve_fit
import numpy as np
import matplotlib.lines as mlines
from matplotlib.path import Path
from itertools import compress

class TestCore(unittest.TestCase):

    ##################################
    ## Matrix test
    ##################################    
    
    def test_matrix_init(self):
        binx = 100
        biny = 100
        w = 0*np.random.random_sample((binx,biny))
        self.assertEqual(np.linalg.det(w), 0, "Should be 0")

    ##################################
    ## Dataframe test
    ##################################    
        
    def create_df(self):
        df = pd.DataFrame(np.random.randn(100, 2))
        return df
    
    def test_df(self):
        d1 = self.create_df()
        d2 = self.create_df()
        # df is equal to itself
        assert_frame_equal(d1,d1)
        # df1 and df2 has same size
        self.assertEqual(len(d1[0]), len(d2[0]), "Should be 100")
        self.assertEqual(len(d1[1]), len(d2[1]), "Should be 2")        

        
    def test_df_content(self):
        def convert(version):
            l = [int(x, 10) for x in version.split('.')]
            l.reverse()
            version = sum(x * (10 ** i) for i, x in enumerate(l))
            return version
            
        df1=pd.DataFrame({'a':[1,2,3,4,5]})
        df2=pd.DataFrame({'a':[6,7,8,9,10]})
        expected_res=pd.Series([7,9,11,13,15])

        v1 = convert(pd.__version__)
        v2 = convert("0.20.0")
        print(v1,v2)
        if v1 > v2:
            assert_series_equal((df1['a']+df2['a']),expected_res,check_names=False)

    def test_df_selection(self):
        df1 = pd.DataFrame(
            {'name': ["faa", "fee", "fii", "foo", "fuu"],
             'value' : [1, 2, 3, 4, 5]
             })
        select = df1['name'] == "foo"
        df = df1.loc[select]
        value = df.iloc[0]['value']
        self.assertEqual(value, 4, "Should be 4")

    ##################################
    ## Gate test
    ##################################

    def create_1D_gate(self):
        xs = []
        ys = []
        poly = mlines.Line2D([],[])
        # slice in the interval [0,10]
        xs.append([0,0])
        ys.append([0,5])        
        xs.append([5,0])
        ys.append([5,5])                
        poly.set_data(xs, ys)
        return poly
    
    def test_create_1D_gate(self):
        poly = self.create_1D_gate()
        # find interval
        xmin = (poly.get_xdata())[0][0]
        xmax = (poly.get_xdata())[1][0]        
        self.assertEqual(xmin,0)
        self.assertEqual(xmax,5)        
                       
    def test_create_2D_gate(self):
        # contour with vertices [0,0] [10,0] [10,10] [0,10]
        p = Path([(0,0), (0, 10), (10, 10), (10, 0)])  
        point = [[5,5]]
        isInside = p.contains_points(point)
        self.assertEqual(isInside,True)
        
    ##################################
    ## Integration test
    ##################################    

    def test_integ_1D(self):
        poly = self.create_1D_gate()
        # find interval
        xmin = (poly.get_xdata())[0][0]
        xmax = (poly.get_xdata())[1][0]

        X = [1,2,3,4,5,6,7,8,9,10]
        Y = [1,2,3,4,5,6,7,8,9,10]
        index_list = [i for i, e in enumerate(X) if e > xmin and e <= xmax]
        ss = sum(Y[index_list[0]:index_list[-1]])
        self.assertEqual(ss, 10)
        
    def test_integ_2D(self):        
        x = [1,2,11,22]
        y = [1,2,11,22]
        z = [1,1,1,1]
        # contour with vertices [0,0] [10,0] [10,10] [0,10]
        p = Path([(0,0), (0, 10), (10, 10), (10, 0)])
        points = [[1,1],[2,2],[11,11],[22,22]]
        isInside = p.contains_points(points)
        x_sel = list(compress(x, isInside))
        y_sel = list(compress(y, isInside))
        z_sel = list(compress(z, isInside))        
        self.assertEqual(sum(z_sel), 2)
        
    ##################################
    ## Fit test
    ##################################    

    def test_scipy(self):            

        def testF(x, a, b): 
            return a + b * x
        
        x = np.linspace(0, 10, num = 20)
        y = np.linspace(0, 10, num = 20)        
        p0 = sy.array([0,1])
        param, param_cov = curve_fit(testF, x, y, p0)

        if (param[0] - p0[0])< 0.0001:
            param[0] = 0
        if (param[1] - p0[1])< 0.0001:
            param[1] = 1            
        self.assertEqual(param[0], 0)
        self.assertEqual(param[1], 1)        

if __name__ == '__main__':
    unittest.main()
