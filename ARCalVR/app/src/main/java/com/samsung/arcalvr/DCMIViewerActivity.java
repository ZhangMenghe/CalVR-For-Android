package com.samsung.arcalvr;

import com.samsung.arcalvr.Utils.dcmImage;
import com.samsung.arcalvr.Utils.fileUtils;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

public class DCMIViewerActivity extends MainActivity{
    @Override
    protected void setupResource(){
        super.setupResource();
        //setupDCMI
        List<String> file_path_lst = fileUtils.getListFilesFromDir(
                new File(calvr_dest +"/resources/dcm/"));
        ArrayList<dcmImage> dcm_images = new ArrayList<>();

        for(String file_path : file_path_lst)
            dcm_images.add(new dcmImage(file_path));

        //pass down to native
        JniInterface.JNIsendDCMImgs(dcm_images.toArray(new dcmImage[0]), file_path_lst.size());
    }
}
