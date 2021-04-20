import random
def execute(self, inputs, outputs, gvm):
    x = inputs['input_0']
    active = inputs['input_2']
    if active == 2:
        x = 0.0
    if active == 1:
        outputs['output_1'] = x + 0.03 + random.uniform(-0.015, 0.015)
    else:
        outputs['output_1'] = x
    self.logger.info("Hello {}".format(self.name))
    self.logger.info(x)
    #self.preemptive_wait(1)
    return "success"
