
def execute(self, inputs, outputs, gvm):
    progress_1 = inputs['input_0']
    progress_2 = inputs['input_1']
    f = open("data_1.txt", "a")
    f.write(str(progress_1))
    f.write(",")
    f.write(str(progress_2))
    f.write("\n")

    outputs['output_2'] = 1
    outputs['output_3'] = 1
    delta = 1
    
    if (progress_1 > (progress_2 + delta) or progress_1 >=1.0):
        outputs['output_2'] = 0
    if (progress_2 > (progress_1 + delta) or progress_2 >=1.0):
        outputs['output_3'] = 0
    if (progress_1 >=1.0 and progress_2 >=1.0):
        #reset
        outputs['output_2'] = 2
        outputs['output_3'] = 2
        f.write("-\n")
        self.logger.info("RESET")
        
            
    f.close()    

    self.logger.info("Hello {}".format(self.name))
    self.preemptive_wait(0.1)
    return "success"
