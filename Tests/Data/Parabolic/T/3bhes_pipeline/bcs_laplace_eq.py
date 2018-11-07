
import OpenGeoSys
import numpy as np
from tespy import  cmp, con, nwk, hlp
def solution(t):
    # %% network
    btes = nwk.network(fluids=['water'],
                         T_unit='C', p_unit='bar', h_unit='kJ / kg',
                         T_range=[0.1, 100], p_range=[1, 100], h_range=[1, 1000])

    # components

    # cycle closer (from consumer)
    fc_in = cmp.source('from consumer inflow')
    fc_out = cmp.sink('from consumer outflow')
    
    p1 = cmp.pump('pump')

    sp = cmp.splitter('splitter')

    #bhe:
    bhe1 = cmp.heat_exchanger_simple('bhe1')
    bhe2 = cmp.heat_exchanger_simple('bhe2')
    bhe3 = cmp.heat_exchanger_simple('bhe3')

    mg = cmp.merge('merge')

    cons = cmp.heat_exchanger_simple('consumer')
    # two btes subsystems (pipes in parallel flow):
    # 10 pipes for outer ring, 5 pipes inner ring
    #bo = bp('btes outer', 4)
    #bi = bp('btes inner', 2)

    # circulation pump and consumer
    #pump = cmp.pump('circulation pump')
    #cons = cmp.heat_exchanger_simple('consumer')

    #connections

    # inlet
    fc_p1 = con.connection(fc_in, 'out1', p1, 'in1')

    p1_sp = con.connection(p1, 'out1', sp, 'in1')
    
    sp_bhe1 = con.connection(sp, 'out1', bhe1, 'in1')
    sp_bhe2 = con.connection(sp, 'out2', bhe2, 'in1')

    bhe2_bhe3 = con.connection(bhe2, 'out1', bhe3, 'in1')
    
    bhe1_mg = con.connection(bhe1, 'out1', mg, 'in1')
    bhe3_mg = con.connection(bhe3, 'out1', mg, 'in2')

    mg_cons = con.connection(mg, 'out1', cons, 'in1')

    cons_fc = con.connection(cons, 'out1', fc_out, 'in1')

    btes.add_conns(fc_p1, p1_sp, sp_bhe1, sp_bhe2, bhe2_bhe3, bhe1_mg, bhe3_mg, mg_cons, cons_fc)

    #busses

    #heat = con.bus('consumer heat demand')
    #heat.add_comps([cons, 1])
    #btes.add_busses(heat)

    #component parameters

    # provide volumetric flow in m^3 / s
    x = np.array([0.00, 0.001952885971862, 0.00390577194372, 0.005858657915586,
                  0.007811543887448, 0.00976442985931, 0.011717315831173,
                  0.013670201803035, 0.015623087774897, 0.017575973746759,
                  0.019528859718621, 0.021481745690483, 0.023434631662345,
                  0.025387517634207, 0.027340403606069, 0.029293289577931,
                  0.031246175549793, 0.033199061521655, 0.035151947493517,
                  0.037104833465379, 0.039057719437241, 0.041010605409104,
                  0.042963491380966, 0.044916377352828, 0.04686926332469,
                  0.048822149296552, 0.050775035268414, 0.052727921240276,
                  0.054680807212138, 0.056633693184
                  ])

    # provide head in Pa
    y = np.array([0.47782539, 0.47725723, 0.47555274, 0.47271192, 0.46873478,
                  0.46362130, 0.45737151, 0.44998538, 0.44146293, 0.43180416,
                  0.42100905, 0.40907762, 0.39600986, 0.38180578, 0.36646537,
                  0.34998863, 0.33237557, 0.31362618, 0.29374046, 0.27271841,
                  0.25056004, 0.22726535, 0.20283432, 0.17726697, 0.15056329,
                  0.12272329, 0.09374696, 0.06363430, 0.03238531, 0.00000000
                  ])*1e5
    # automatic unit conversion not implemented yet!

    # flow_char creates a look-up-table, linear interpolation inbetween the points
    # you need to import hlp for this to work. dc_cc is a data container class
    # with all required properties for characteristic functions
    
    f = hlp.dc_cc(x=x, y=y, is_set=True)
    p1.set_attr( flow_char=f)

    # system inlet
    #fc_p1.set_attr(p=1, T=10, fluid={'water': 1})
    fc_p1.set_attr(p=2, T=10, fluid={'water': 1})

    #p1_sp.set_attr(h=con.ref(fc_p1, 1, 0))
    #parametrization
    #p1.set_attr(pr=1.5, Q =0)

    #bhe1.set_attr(D=0.02733,L=100,ks=100,hydro_group ='HW', Q =0)
    #bhe2.set_attr(D=0.02733,L=100,ks=100,hydro_group ='HW', Q =0)
    #bhe3.set_attr(D=0.02733,L=100,ks=100,hydro_group ='HW', Q =0)

    bhe1.set_attr(D=0.02733,L=100,ks=100,hydro_group ='HW')
    bhe2.set_attr(D=0.02733,L=100,ks=100,hydro_group ='HW')
    bhe3.set_attr(D=0.02733,L=100,ks=100,hydro_group ='HW')

    cons.set_attr(D=0.02733,L=100,ks=100,hydro_group ='HW', Q =-3000)
    #cons.set_attr(D=0.02733,L=100,ks=100,hydro_char ='HW', P=-18e3)

    #connection temperature parametrization
    #Tin:
    p1_sp.set_attr(T= 10)
    bhe2_bhe3.set_attr(T= t + 11)

    #Tout:
    bhe1_mg.set_attr(T= 11)
    bhe3_mg.set_attr(T= 12)

    # closing the circle
    cons_fc.set_attr(p=con.ref(fc_p1, 1, 0))
    # consumer heat demand

    # solving
    btes.solve('design')
    #get results:
    t_bhe2bhe3 = bhe2_bhe3.get_attr('T').val_SI

    #return value to OGS
    return t_bhe2bhe3

# %%
# Dirichlet BCs
class BC(OpenGeoSys.BoundaryCondition):
    def getDirichletBCValue(self, t, coords, node_id, primary_vars):

        t = primary_vars[0]
        value = solution(t)
        return (True, value)

bc = BC()
